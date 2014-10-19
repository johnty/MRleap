
/************************************/
#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object
#include "ext_proto.h" 
#include "Leap.h"
#include <math.h>
#include "LeapMath.h"

#include "jit.common.h"

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

#define CELL_PTR_1D(info, data, x)         (((float *)(data)) + ((info)->dimstride[0] * (x)))
#define CELL_PTR_2D(info, data, x, y)      (CELL_PTR_1D (info, data, x) + ((info)->dimstride[1] * (y)))
/************************************/
// a wrapper for cpost() only called for debug builds on Windows
// to see these console posts, run the DbgView program (part of the SysInternals package distributed by Microsoft)
#if defined( NDEBUG ) || defined( MAC_VERSION )
#define DPOST
#else
#define DPOST cpost
#endif

// a macro to mark exported symbols in the code without requiring an external file to define them
#ifdef WIN_VERSION
	// note that this is the required syntax on windows regardless of whether the compiler is msvc or gcc
	#define T_EXPORT __declspec(dllexport)
#else // MAC_VERSION
	// the mac uses the standard gcc syntax, you should also set the -fvisibility=hidden flag to hide the non-marked symbols
	#define T_EXPORT __attribute__((visibility("default")))
#endif

/************************************/
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};
/************************************/
typedef struct _MRleap
{
	t_object            ob;
    void                *m_clock;
    
    long                prevFrameID;
    long                curFrameID;
    
    long                allDegOnOff;
    ///////////image
    long                imageOnOff;
    long                imageDistortionOnOff;
    ///////////frame
    long                frameRotOnOff;
    long                frameMainOnOff;
    long                frameRotRawOnOff;
    long                frameScaleOnOff;
    long                frameScaleRawOnOff;
    long                frameTranslationOnOff;
    long                frameTranslationRawOnOff;
    long                frameTranslationNormOnOff;
    long                frameRotationMatrix;
    
    float               frameRotProb;
    float               frameScaleProb;
    float               frameTranslationProb;
    
    long                frameHist;
    ///////////hand
    long                handMainOnOff;
    long                handSphereOnOff;
    long                handSphereNorm;
    long                handPositionOnOff;
    long                handPositionNormOnOff;
    long                handPositionStabilizationOnOff;
    long                handRotationOnOff;
    long                handRotationRawOnOff;
    long                handScaleRawOnOff;
    long                handScaleOnOff;
    long                handTranslationRawOnOff;
    long                handTranslationNormOnOff;
    long                handTranslationOnOff;
    long                handRotationMatrix;
    long                handHorizontalPlaneOnOff;
    long                handPositionVelocityOnOff;
    ///////////2.x stuff
    long                handBasisOnOff;
    ////////////////
    
    float               handRotationProb;
    float               handScaleProb;
    float               handTranslationProb;
    //////Arm
    long                armBasisOnOff;
    long                armCenterOnOff;
    long                armCenterNormOnOff;
    long                armDirectionOnOff;
    long                armElbowPositionOnOff;
    long                armElbowPositionNormOnOff;
    long                armWristPositionOnOff;
    long                armWristPositionNormOnOff;
    long                armWidthOnOff;
    //////pointables
    long                toolMainOnOff;
    long                fingerMainOnOff;
    long                toolDirectionOnOff;
    long                fingerDirectionOnOff;
    long                toolTipPositionOnOff;
    long                toolTipNormOnOff;
    long                fingerTipPositionOnOff;
    long                fingerTipNormOnOff;
    long                toolTipVelocityOnOff;
    long                fingerTipVelocityOnOff;
    long                toolDimensionOnOff;
    long                fingerDimensionOnOff;
    long                toolTouchZoneOnOff;
    long                fingerTouchZoneOnOff;
    //////fingers 2x
    long                fingerIsExtendedOnOff;
    ///////////////
    /////outlets
    void                *outletStart;
	void                *outletFrame;
    void                *outletHands;
    void                *outletArms;
    void                *outletTools;
    void                *outletFingers;
    void                *outletGestures;
    //////////////
    ///////Gesture attr set
    float               circleMinRadius;
    float               circleMinArc;
    float               swipeMinLength;
    float               swipeMinVelocity;
    float               keyTapMinDownVelocity;
    float               keyTapHistorySeconds;
    float               keyTapMinDistance;
    float               screenTapMinForwardVelocity;
    float               screenTapHistorySeconds;
    float               screenTapMinDistance;
    //////////////
    //////Gesture
    long                gestureCircleOnOff;
    long                gestureCircleMainOnOff;
    long                gestureCircleCenterOnOff;
    long                gestureCircleCenterNormOnOff;
    long                gestureCircleDataOnOff;
    //------------------
    long                gestureKeyTapOnOff;
    long                gestureKeyTapMainOnOff;
    long                gestureKeyTapPositionOnOff;
    long                gestureKeyTapPositionNormOnOff;
    long                gestureKeyTapDirectionOnOff;
    //------------------
    long                gestureScreenTapOnOff;
    long                gestureScreenTapMainOnOff;
    long                gestureScreenTapPositionOnOff;
    long                gestureScreenTapPositionNormOnOff;
    long                gestureScreenTapDirectionOnOff;
    //------------------
    long                gestureSwipeOnOff;
    long                gestureSwipeMainOnOff;
    long                gestureSwipePositionOnOff;
    long                gestureSwipePositionNormOnOff;
    long                gestureSwipeDirectionOnOff;
    //------------------
    ////default gesture values
    float circleMinRadiusDef;
    float circleMinArcDef;
    float swipeMinLengthDef;
    float swipeMinVelocityDef;
    float keyTapMinDownVelocityDef;
    float keyTapHistorySecondsDef;
    float keyTapMinDistanceDef;
    float screenTapMinForwardVelocityDef;
    float screenTapHistorySecondsDef;
    float screenTapMinDistanceDef;
    
    ///////////
    //////Leap
	Leap::Controller	*leap;
    ////////////////
    
    t_symbol            *RIGHT;
    t_symbol            *LEFT;
    t_symbol            *HAND;
    t_symbol            *OTHER;
    
    ///////genSym
    /*If you'll be sending the same message a lot, you might call gensym() on the message string at
     initialization time and store the result in a global variable to save the
     (significant) overhead of calling gensym() every time you want to send a
     message.
    */
    
    t_symbol            *s_frameMain;
    t_symbol            *s_x;
    t_symbol            *s_y;
    t_symbol            *s_z;
    t_symbol            *s_rotation;
    t_symbol            *s_rotationRaw;
    t_symbol            *s_scale;
    t_symbol            *s_scaleRaw;
    t_symbol            *s_translation;
    t_symbol            *s_translationRaw;
    t_symbol            *s_rotationMatrix;
    t_symbol            *s_handMain;
    t_symbol            *s_position;
    t_symbol            *s_sphere;
    t_symbol            *s_plane;
    t_symbol            *s_toolMain;
    t_symbol            *s_fingerMain;
    t_symbol            *s_direction;
    t_symbol            *s_velocity;
    t_symbol            *s_tip;
    t_symbol            *s_tipVelocity;
    t_symbol            *s_tipDimension;
    t_symbol            *s_touchZone;
    t_symbol            *s_circleMain;
    t_symbol            *s_circleCenter;
    t_symbol            *s_circleData;
    t_symbol            *s_keyTapMain;
    t_symbol            *s_keyTapPosition;
    t_symbol            *s_keyTapDirection;
    t_symbol            *s_screenTapMain;
    t_symbol            *s_screenTapPosition;
    t_symbol            *s_screenTapDirection;
    t_symbol            *s_swipeMain;
    t_symbol            *s_swipePosition;
    t_symbol            *s_swipeDirection;
    t_symbol            *s_basis;
    t_symbol            *s_center;
    t_symbol            *s_elbowPosition;
    t_symbol            *s_wristPosition;
    t_symbol            *s_width;
    t_symbol            *s_extended;
    
    t_symbol            *fingerNames[5];
    /****************************************************/
    /****************************************************/
    //          JITTER STUFF
    
    t_symbol			*matrix_nameLeft;
    t_symbol			*matrix_nameRight;
    long				plane;
    long				offsetcount;
    long 				offset[JIT_MATRIX_MAX_DIMCOUNT];
    long                imageBufferLength;
    long                prevDim[2];
    t_symbol            *prevMatrix;
    /****************************************************/
    /****************************************************/
    
    
} t_MRleap;

/************************************/
void *MRleap_new(t_symbol *s, long argc, t_atom *argv);
void MRleap_free(t_MRleap *x);
void MRleap_assist(t_MRleap *x, void *b, long m, long a, char *s);
float MRleap_RadDeg(t_MRleap *x);
float MRleap_DegRad(t_MRleap *x);
Leap::Vector MRleap_normalizeVec(t_MRleap *x, Leap::Frame frame, Leap::Vector vec, long is);
void MRleap_assignHandID(t_MRleap *x, Leap::Hand hand);

void MRleap_getFrameData(t_MRleap *x,           Leap::Frame frame);
void MRleap_getHandData(t_MRleap *x,            Leap::Frame frame);
void MRleap_getToolData(t_MRleap *x,            Leap::Frame frame);
void MRleap_getFingerData(t_MRleap *x,          Leap::Frame frame);
void MRleap_getGestureData(t_MRleap *x,         Leap::Frame frame);
void MRleap_processCircleData(t_MRleap *x,      Leap::Frame frame, Leap::Gesture gesture);
void MRleap_processKeyTapData(t_MRleap *x,      Leap::Frame frame, Leap::Gesture gesture);
void MRleap_processScreenTapData(t_MRleap *x,   Leap::Frame frame, Leap::Gesture gesture);
void MRleap_processSwipeData(t_MRleap *x,       Leap::Frame frame, Leap::Gesture gesture);
void MRleap_getArmData(t_MRleap *x,             Leap::Frame frame);

void MRleap_gestureResetAll(t_MRleap *x);
void MRleap_circleGestureReset(t_MRleap *x);
void MRleap_keyTapGestureReset(t_MRleap *x);
void MRleap_screenTapGestureReset(t_MRleap *x);
void MRleap_swipeGestureReset(t_MRleap *x);
void MRleap_gestureResetGeneric(t_MRleap *x, t_symbol *gesture);
/****************************************************/
/****************************************************/
//          JITTER STUFF

void MRleap_getImageData(t_MRleap *x,           Leap::Frame frame);
void MRleap_getImage    (t_MRleap *x,           Leap::Image image, t_symbol *matrixName);
void MRleap_getDistortion(t_MRleap *x,          Leap::Image image);
 /****************************************************/
 /****************************************************/
////gesture attributes
t_max_err MRleap_gestureCircleMinArc_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureCircleMinRadius_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureSwipeMinLength_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureSwipeMinVelocity_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureKeyTapMinDownVelocity_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureKeyTapMinDistance_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureKeyTapHistorySeconds_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureScreenTapMinForwardVelocity_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureScreenTapHistorySeconds_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureScreenTapMinDistance_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
//////////
t_max_err MRleap_gestureCircleOnOff_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureKeyTapOnOff_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureScreenTapOnOff_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureSwipeOnOff_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);

int MRleap_biggestRotAxis(t_MRleap *x, Leap::Vector vec);

void MRleap_bang(t_MRleap *x);
void MRleap_checkLeapConnection(t_MRleap *x);

/************************************/
static t_class *MRleap_class;


/************************************/
int T_EXPORT main(void)
{

	t_class *c;
    
	c = class_new("MRleap", (method)MRleap_new, (method)MRleap_free, sizeof(t_MRleap), NULL,         A_GIMME,   0);
    class_addmethod(c,      (method)MRleap_bang,                     "bang",                                    0);
    class_addmethod(c,      (method)MRleap_assist,                   "assist",                       A_CANT,    0);
    class_addmethod(c,      (method)MRleap_gestureResetAll,          "resetGestureAll",              A_NOTHING, 0);
    class_addmethod(c,      (method)MRleap_circleGestureReset,       "resetGestureCircle",           A_NOTHING, 0);
    class_addmethod(c,      (method)MRleap_keyTapGestureReset,       "resetGestureKeyTap",           A_NOTHING, 0);
    class_addmethod(c,      (method)MRleap_screenTapGestureReset,    "resetGestureScreenTap",        A_NOTHING, 0);
    class_addmethod(c,      (method)MRleap_swipeGestureReset,        "resetGestureSwipe",            A_NOTHING, 0);
    class_addmethod(c,      (method)MRleap_gestureResetGeneric,      "resetGesture",                 A_SYM,     0);
    
//    post("%d", class_alias(c, gensym("MR.leap")));
    //////////////attribute
    /***************************Global***********************************/
    CLASS_ATTR_LONG(c, "frameHist", 0, t_MRleap, frameHist);
	CLASS_ATTR_LABEL(c, "frameHist", 0, "FrameHist");
    CLASS_ATTR_FILTER_CLIP(c, "frameHist", 1, 59);
	CLASS_ATTR_SAVE(c,"frameHist", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "allDegOnOff", 0, t_MRleap, allDegOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "allDegOnOff", 0, "onoff", "DegreesOnOff");
	CLASS_ATTR_SAVE(c,"allDegOnOff", 0);
    /************************** IMAGE ********************************/
    CLASS_ATTR_LONG(c, "imageOnOff", 0, t_MRleap, imageOnOff);
    CLASS_ATTR_STYLE_LABEL(c, "imageOnOff", 0, "onoff", "ImageOnOff");
    CLASS_ATTR_SAVE(c,"imageOnOff", 0);
    /////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "imageDistortionOnOff", 0, t_MRleap, imageDistortionOnOff);
    CLASS_ATTR_STYLE_LABEL(c, "imageDistortionOnOff", 0, "onoff", "ImageDistortionOnOff");
    CLASS_ATTR_SAVE(c,"imageDistortionOnOff", 0);
    
/*    CLASS_ATTR_SYM(c, "matrix_nameLeft", 0, t_MRleap, matrix_nameLeft);
    CLASS_ATTR_STYLE_LABEL(c, "matrix_nameLeft", 0, "text", "Matrix_nameLeft");
    CLASS_ATTR_SAVE(c,"matrix_nameLeft", 0);

    CLASS_ATTR_SYM(c, "matrix_nameRight", 0, t_MRleap, matrix_nameRight);
    CLASS_ATTR_STYLE_LABEL(c, "matrix_nameRight", 0, "text", "Matrix_nameRight");
    CLASS_ATTR_SAVE(c,"matrix_nameRight", 0);
*/
    /************************** FRAME ********************************/
    CLASS_ATTR_LONG(c, "frameMainOnOff", 0, t_MRleap, frameMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameMainOnOff", 0, "onoff", "FrameMainOnOff");
	CLASS_ATTR_SAVE(c,"frameMainOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "frameRotOnOff", 0, t_MRleap, frameRotOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameRotOnOff", 0, "onoff", "FrameRotOnOff");
	CLASS_ATTR_SAVE(c,"frameRotOnOff", 0);
   
	CLASS_ATTR_LONG(c, "frameRotRawOnOff", 0, t_MRleap, frameRotRawOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameRotRawOnOff", 0, "onoff", "FrameRotRawOnOff");
	CLASS_ATTR_SAVE(c,"frameRotRawOnOff", 0);
    
    CLASS_ATTR_FLOAT(c, "frameRotProb", 0, t_MRleap, frameRotProb);
	CLASS_ATTR_LABEL(c, "frameRotProb", 0, "FrameRotProbOnOff");
    CLASS_ATTR_FILTER_CLIP(c, "frameRotProb", 0, 1);
	CLASS_ATTR_SAVE(c,"frameRotProb", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "frameScaleOnOff", 0, t_MRleap, frameScaleOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameScaleOnOff", 0, "onoff", "FrameScaleOnOff");
	CLASS_ATTR_SAVE(c,"frameScaleOnOff", 0);

	CLASS_ATTR_LONG(c, "frameScaleRawOnOff", 0, t_MRleap, frameScaleRawOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameScaleRawOnOff", 0, "onoff", "FrameScaleRawOnOff");
	CLASS_ATTR_SAVE(c,"frameScaleRawOnOff", 0);

    CLASS_ATTR_FLOAT(c, "frameScaleProb", 0, t_MRleap, frameScaleProb);
	CLASS_ATTR_LABEL(c, "frameScaleProb", 0, "FrameScaleProbOnOff");
    CLASS_ATTR_FILTER_CLIP(c, "frameScaleProb", 0, 1);
	CLASS_ATTR_SAVE(c,"frameScaleProb", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "frameTranslationOnOff", 0, t_MRleap, frameTranslationOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameTranslationOnOff", 0, "onoff", "FrameTransOnOff");
	CLASS_ATTR_SAVE(c,"frameTranslationOnOff", 0);

    CLASS_ATTR_LONG(c, "frameTranslationRawOnOff", 0, t_MRleap, frameTranslationRawOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameTranslationRawOnOff", 0, "onoff", "FrameTransRawOnOff");
	CLASS_ATTR_SAVE(c,"frameTranslationRawOnOff", 0);
     
    CLASS_ATTR_LONG(c, "frameTranslationNormOnOff", 0, t_MRleap, frameTranslationNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameTranslationNormOnOff", 0, "onoff", "FrameTransNormOnOff");
	CLASS_ATTR_SAVE(c,"frameTranslationNormOnOff", 0);
    
    CLASS_ATTR_FLOAT(c, "frameTranslationProb", 0, t_MRleap, frameTranslationProb);
	CLASS_ATTR_LABEL(c, "frameTranslationProb", 0, "FrameTransProbOnOff");
    CLASS_ATTR_FILTER_CLIP(c, "frameTranslationProb", 0, 1);
	CLASS_ATTR_SAVE(c,"frameTranslationProb", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "frameRotationMatrixOnOff", 0, t_MRleap, frameRotationMatrix);
	CLASS_ATTR_STYLE_LABEL(c, "frameRotationMatrixOnOff", 0, "onoff", "FrameRotMatrixOnOff");
	CLASS_ATTR_SAVE(c,"frameRotationMatrixOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    /*********************************************************************/
    /************************** HAND *************************************/
    CLASS_ATTR_LONG(c, "handMainOnOff", 0, t_MRleap, handMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handMainOnOff", 0, "onoff", "HandMainOnOff");
	CLASS_ATTR_SAVE(c,"handMainOnOff", 0);
    //////////////////////////////////////////////////////////////////////
	CLASS_ATTR_LONG(c, "handSphereOnOff", 0, t_MRleap, handSphereOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handSphereOnOff", 0, "onoff", "HandSphereOnOff");
	CLASS_ATTR_SAVE(c,"handSphereOnOff", 0);

    CLASS_ATTR_LONG(c, "handSphereNormOnOff", 0, t_MRleap, handSphereNorm);
	CLASS_ATTR_STYLE_LABEL(c, "handSphereNormOnOff", 0, "onoff", "HandSphereNormOnOff");
	CLASS_ATTR_SAVE(c,"handSphereNormOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "handPositionOnOff", 0, t_MRleap, handPositionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handPositionOnOff", 0, "onoff", "HandPositionOnOff");
	CLASS_ATTR_SAVE(c,"handPositionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "handPositionNormOnOff", 0, t_MRleap, handPositionNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handPositionNormOnOff", 0, "onoff", "HandPosNormOnOff");
	CLASS_ATTR_SAVE(c,"handPositionNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "handPositionVelocityOnOff", 0, t_MRleap, handPositionVelocityOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handPositionVelocityOnOff", 0, "onoff", "HandVelocityOnOff");
	CLASS_ATTR_SAVE(c,"handPositionVelocityOnOff", 0);
    
    CLASS_ATTR_LONG(c, "handPositionStabilizationOnOff", 0, t_MRleap, handPositionStabilizationOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handPositionStabilizationOnOff", 0, "onoff", "HandPosStableOnOff");
	CLASS_ATTR_SAVE(c,"handPositionStabilizationOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "handRotationOnOff", 0, t_MRleap, handRotationOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handRotationOnOff", 0, "onoff", "HandRotOnOff");
	CLASS_ATTR_SAVE(c,"handRotationOnOff", 0);

	CLASS_ATTR_LONG(c, "handRotationRawOnOff", 0, t_MRleap, handRotationRawOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handRotationRawOnOff", 0, "onoff", "HandRotRawOnOff");
	CLASS_ATTR_SAVE(c,"handRotationRawOnOff", 0);
    
    CLASS_ATTR_FLOAT(c, "handRotationProb", 0, t_MRleap, handRotationProb);
	CLASS_ATTR_LABEL(c, "handRotationProb", 0, "HandRotProbOnOff");
    CLASS_ATTR_FILTER_CLIP(c, "handRotationProb", 0, 1);
	CLASS_ATTR_SAVE(c,"handRotationProb", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "handScaleOnOff", 0, t_MRleap, handScaleOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handScaleOnOff", 0, "onoff", "HandScaleOnOff");
	CLASS_ATTR_SAVE(c,"handScaleOnOff", 0);

	CLASS_ATTR_LONG(c, "handScaleRawOnOff", 0, t_MRleap, handScaleRawOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handScaleRawOnOff", 0, "onoff", "HandScaleRawOnOff");
	CLASS_ATTR_SAVE(c,"handScaleRawOnOff", 0);

    CLASS_ATTR_FLOAT(c, "handScaleProb", 0, t_MRleap, handScaleProb);
	CLASS_ATTR_LABEL(c, "handScaleProb", 0, "HandScaleProbOnOff");
    CLASS_ATTR_FILTER_CLIP(c, "handScaleProb", 0, 1);
	CLASS_ATTR_SAVE(c,"handScaleProb", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "handTranslationOnOff", 0, t_MRleap, handTranslationOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handTranslationOnOff", 0, "onoff", "HandTransOnOff");
	CLASS_ATTR_SAVE(c,"handTranslationOnOff", 0);

    CLASS_ATTR_LONG(c, "handTranslationRawOnOff", 0, t_MRleap, handTranslationRawOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handTranslationRawOnOff", 0, "onoff", "HandTransRawOnOff");
	CLASS_ATTR_SAVE(c,"handTranslationRawOnOff", 0);

    CLASS_ATTR_LONG(c, "handTranslationNormOnOff", 0, t_MRleap, handTranslationNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handTranslationNormOnOff", 0, "onoff", "HandTransNormOnOff");
	CLASS_ATTR_SAVE(c,"handTranslationNormOnOff", 0);

    CLASS_ATTR_FLOAT(c, "handTranslationProb", 0, t_MRleap, handTranslationProb);
	CLASS_ATTR_LABEL(c, "handTranslationProb", 0, "HandTransProbOnOff");
    CLASS_ATTR_FILTER_CLIP(c, "handTranslation  Prob", 0, 1);
	CLASS_ATTR_SAVE(c,"handTranslationProb", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "handRotationMatrixOnOff", 0, t_MRleap, handRotationMatrix);
	CLASS_ATTR_STYLE_LABEL(c, "handRotationMatrixOnOff", 0, "onoff", "HandRotMatrixOnOff");
	CLASS_ATTR_SAVE(c,"handRotationMatrixOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "handHorizontalPlaneOnOff", 0, t_MRleap, handHorizontalPlaneOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handHorizontalPlaneOnOff", 0, "onoff", "HandPlaneOnOff");
	CLASS_ATTR_SAVE(c,"handHorizontalPlaneOnOff", 0);
    /////////////////2.x stuff
    CLASS_ATTR_LONG(c, "handBasisOnOff", 0, t_MRleap, handBasisOnOff);
    CLASS_ATTR_STYLE_LABEL(c, "handBasisOnOff", 0, "onoff", "HandBasisOnOff");
    CLASS_ATTR_SAVE(c,"handBasisOnOff", 0);
    /////////////////////
    /******************************ARM*************************************/
    CLASS_ATTR_LONG(c, "armBasisOnOff", 0, t_MRleap, armBasisOnOff);
    CLASS_ATTR_STYLE_LABEL(c, "armBasisOnOff", 0, "onoff", "ArmBasisOnOff");
    CLASS_ATTR_SAVE(c,"armBasisOnOff", 0);
    
    CLASS_ATTR_LONG(c, "armCenterOnOff", 0, t_MRleap, armCenterOnOff);
    CLASS_ATTR_STYLE_LABEL(c, "armCenterOnOff", 0, "onoff", "ArmCenterOnOff");
    CLASS_ATTR_SAVE(c,"armCenterOnOff", 0);
    
    CLASS_ATTR_LONG(c, "armCenterNormOnOff", 0, t_MRleap, armCenterNormOnOff);
    CLASS_ATTR_STYLE_LABEL(c, "armCenterNormOnOff", 0, "onoff", "ArmCenterNormOnOff");
    CLASS_ATTR_SAVE(c,"armCenterNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "armDirectionOnOff", 0, t_MRleap, armDirectionOnOff);
    CLASS_ATTR_STYLE_LABEL(c, "armDirectionOnOff", 0, "onoff", "ArmDirectionOnOff");
    CLASS_ATTR_SAVE(c,"armDirectionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "armElbowPositionOnOff", 0, t_MRleap, armElbowPositionOnOff);
    CLASS_ATTR_STYLE_LABEL(c, "armElbowPositionOnOff", 0, "onoff", "ArmElbowPositionOnOff");
    CLASS_ATTR_SAVE(c,"armElbowPositionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "armElbowPositionNormOnOff", 0, t_MRleap, armElbowPositionNormOnOff);
    CLASS_ATTR_STYLE_LABEL(c, "armElbowPositionNormOnOff", 0, "onoff", "ArmElbowPositionNormOnOff");
    CLASS_ATTR_SAVE(c,"armElbowPositionNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "armWristPositionOnOff", 0, t_MRleap, armWristPositionOnOff);
    CLASS_ATTR_STYLE_LABEL(c, "armWristPositionOnOff", 0, "onoff", "ArmWristPositionOnOff");
    CLASS_ATTR_SAVE(c,"armWristPositionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "armWristPositionNormOnOff", 0, t_MRleap, armWristPositionNormOnOff);
    CLASS_ATTR_STYLE_LABEL(c, "armWristPositionNormOnOff", 0, "onoff", "ArmWristPositionNormOnOff");
    CLASS_ATTR_SAVE(c,"armWristPositionNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "armWidthOnOff", 0, t_MRleap, armWidthOnOff);
    CLASS_ATTR_STYLE_LABEL(c, "armWidthOnOff", 0, "onoff", "ArmWidthOnOff");
    CLASS_ATTR_SAVE(c,"armWidthOnOff", 0);
    /*********************************************************************/
    /*****************************Pointalbes******************************/
    CLASS_ATTR_LONG(c, "toolMainOnOff", 0, t_MRleap, toolMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolMainOnOff", 0, "onoff", "ToolMainOnOff");
	CLASS_ATTR_SAVE(c,"toolMainOnOff", 0);

    CLASS_ATTR_LONG(c, "fingerMainOnOff", 0, t_MRleap, fingerMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerMainOnOff", 0, "onoff", "FingerMainOnOff");
	CLASS_ATTR_SAVE(c,"fingerMainOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "toolDirectionOnOff", 0, t_MRleap, toolDirectionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolDirectionOnOff", 0, "onoff", "ToolDirectionOnOff");
	CLASS_ATTR_SAVE(c,"toolDirectionOnOff", 0);

    CLASS_ATTR_LONG(c, "fingerDirectionOnOff", 0, t_MRleap, fingerDirectionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerDirectionOnOff", 0, "onoff", "FingerDirectionOnOff");
	CLASS_ATTR_SAVE(c,"fingerDirectionOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "toolTipPositionOnOff", 0, t_MRleap, toolTipPositionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolTipPositionOnOff", 0, "onoff", "ToolTipOnOff");
	CLASS_ATTR_SAVE(c,"toolTipPositionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "fingerTipPositionOnOff", 0, t_MRleap, fingerTipPositionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerTipPositionOnOff", 0, "onoff", "FingerTipOnOff");
	CLASS_ATTR_SAVE(c,"fingerTipPositionOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "toolTipNormOnOff", 0, t_MRleap, toolTipNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolTipNormOnOff", 0, "onoff", "ToolTipNormOnOff");
	CLASS_ATTR_SAVE(c,"toolTipNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "fingerTipNormOnOff", 0, t_MRleap, fingerTipNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerTipNormOnOff", 0, "onoff", "FingerTipNormOnOff");
	CLASS_ATTR_SAVE(c,"fingerTipNormOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "toolTipVelocityOnOff", 0, t_MRleap, toolTipVelocityOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolTipVelocityOnOff", 0, "onoff", "ToolTipVelOnOff");
	CLASS_ATTR_SAVE(c,"toolTipVelocityOnOff", 0);

    CLASS_ATTR_LONG(c, "fingerTipVelocityOnOff", 0, t_MRleap, fingerTipVelocityOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerTipVelocityOnOff", 0, "onoff", "FingerTipVelOnOff");
	CLASS_ATTR_SAVE(c,"fingerTipVelocityOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "toolDimensionOnOff", 0, t_MRleap, toolDimensionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolDimensionOnOff", 0, "onoff", "ToolDimOnOff");
	CLASS_ATTR_SAVE(c,"toolDimensionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "fingerDimensionOnOff", 0, t_MRleap, fingerDimensionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerDimensionOnOff", 0, "onoff", "FingerDimOnOff");
	CLASS_ATTR_SAVE(c,"fingerDimensionOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "toolTouchZoneOnOff", 0, t_MRleap, toolTouchZoneOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolTouchZoneOnOff", 0, "onoff", "ToolTouchOnOff");
	CLASS_ATTR_SAVE(c,"toolTouchZoneOnOff", 0);
    
    CLASS_ATTR_LONG(c, "fingerTouchZoneOnOff", 0, t_MRleap, fingerTouchZoneOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerTouchZoneOnOff", 0, "onoff", "FingerTouchOnOff");
	CLASS_ATTR_SAVE(c,"fingerTouchZoneOnOff", 0);
    /****************************Finger 2.x*********************************/
    CLASS_ATTR_LONG(c, "fingerIsExtendedOnOff", 0, t_MRleap, fingerIsExtendedOnOff);
    CLASS_ATTR_STYLE_LABEL(c, "fingerIsExtendedOnOff", 0, "onoff", "FingerIsExtendedOnOff");
    CLASS_ATTR_SAVE(c,"fingerIsExtendedOnOff", 0);
    /*********************************************************************/
    /*************************Gesture enable******************************/
    CLASS_ATTR_LONG(c, "gestureCircleOnOff", 0, t_MRleap, gestureCircleOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureCircleOnOff", 0, "onoff", "CircleGestureOnOff");
    CLASS_ATTR_ACCESSORS(c, "gestureCircleOnOff", NULL, MRleap_gestureCircleOnOff_set);
	CLASS_ATTR_SAVE(c,"gestureCircleOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureKeyTapOnOff", 0, t_MRleap, gestureKeyTapOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureKeyTapOnOff", 0, "onoff", "KeyTapGestureOnOff");
    CLASS_ATTR_ACCESSORS(c, "gestureKeyTapOnOff", NULL, MRleap_gestureKeyTapOnOff_set);
	CLASS_ATTR_SAVE(c,"gestureKeyTapOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureScreenTapOnOff", 0, t_MRleap, gestureScreenTapOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureScreenTapOnOff", 0, "onoff", "ScreenTapGestureOnOff");
    CLASS_ATTR_ACCESSORS(c, "gestureScreenTapOnOff", NULL, MRleap_gestureScreenTapOnOff_set);
	CLASS_ATTR_SAVE(c,"gestureScreenTapOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureSwipeOnOff", 0, t_MRleap, gestureSwipeOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureSwipeOnOff", 0, "onoff", "SwipeGestureOnOff");
    CLASS_ATTR_ACCESSORS(c, "gestureSwipeOnOff", NULL, MRleap_gestureSwipeOnOff_set);
	CLASS_ATTR_SAVE(c,"gestureSwipeOnOff", 0);
    /********************************Gestures*************************************/
    CLASS_ATTR_LONG(c, "gestureCircleMainOnOff", 0, t_MRleap, gestureCircleMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureCircleMainOnOff", 0, "onoff", "CircleMainOnOff");
	CLASS_ATTR_SAVE(c,"gestureCircleMainOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureCircleCenterOnOff", 0, t_MRleap, gestureCircleCenterOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureCircleCenterOnOff", 0, "onoff", "CircleCenterOnOff");
	CLASS_ATTR_SAVE(c,"gestureCircleCenterOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureCircleCenterNormOnOff", 0, t_MRleap, gestureCircleCenterNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureCircleCenterNormOnOff", 0, "onoff", "CircleCenterNormOnOff");
	CLASS_ATTR_SAVE(c,"gestureCircleCenterNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureCircleDataOnOff", 0, t_MRleap, gestureCircleDataOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureCircleDataOnOff", 0, "onoff", "CircleDataOnOff");
	CLASS_ATTR_SAVE(c,"gestureCircleDataOnOff", 0);
    /////////////////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "gestureSwipeMainOnOff", 0, t_MRleap, gestureSwipeMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureSwipeMainOnOff", 0, "onoff", "SwipeMainOnOff");
	CLASS_ATTR_SAVE(c,"gestureSwipeMainOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureSwipePositionOnOff", 0, t_MRleap, gestureSwipePositionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureSwipePositionOnOff", 0, "onoff", "SwipePositionOnOff");
	CLASS_ATTR_SAVE(c,"gestureSwipePositionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureSwipePositionNormOnOff", 0, t_MRleap, gestureSwipePositionNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureSwipePositionNormOnOff", 0, "onoff", "SwipePosNormOnOff");
	CLASS_ATTR_SAVE(c,"gestureSwipePositionNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureSwipeDirectionOnOff", 0, t_MRleap, gestureSwipeDirectionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureSwipeDirectionOnOff", 0, "onoff", "SwipeDirectionOnOff");
	CLASS_ATTR_SAVE(c,"gestureSwipeDirectionOnOff", 0);
    /////////////////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "gestureKeyTapMainOnOff", 0, t_MRleap, gestureKeyTapMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureKeyTapMainOnOff", 0, "onoff", "KeyTapMainOnOff");
	CLASS_ATTR_SAVE(c,"gestureKeyTapMainOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureKeyTapPositionOnOff", 0, t_MRleap, gestureKeyTapPositionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureKeyTapPositionOnOff", 0, "onoff", "KeyTapPositionOnOff");
	CLASS_ATTR_SAVE(c,"gestureKeyTapPositionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureKeyTapPositionNormOnOff", 0, t_MRleap, gestureKeyTapPositionNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureKeyTapPositionNormOnOff", 0, "onoff", "KeyTapPosNormOnOff");
	CLASS_ATTR_SAVE(c,"gestureKeyTapPositionNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureKeyTapDirectionOnOff", 0, t_MRleap, gestureKeyTapDirectionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureKeyTapDirectionOnOff", 0, "onoff", "KeyTapDirectionOnOff");
	CLASS_ATTR_SAVE(c,"gestureKeyTapDirectionOnOff", 0);
    /////////////////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "gestureScreenTapMainOnOff", 0, t_MRleap, gestureScreenTapMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureScreenTapMainOnOff", 0, "onoff", "ScreenTapMainOnOff");
	CLASS_ATTR_SAVE(c,"gestureScreenTapMainOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureScreenTapPositionOnOff", 0, t_MRleap, gestureScreenTapPositionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureScreenTapPositionOnOff", 0, "onoff", "ScreenTapPositionOnOff");
	CLASS_ATTR_SAVE(c,"gestureScreenTapPositionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureScreenTapPositionNormOnOff", 0, t_MRleap, gestureScreenTapPositionNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureScreenTapPositionNormOnOff", 0, "onoff", "ScreenTapPosNormOnOff");
	CLASS_ATTR_SAVE(c,"gestureScreenTapPositionNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureScreenTapDirectionOnOff", 0, t_MRleap, gestureScreenTapDirectionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureScreenTapDirectionOnOff", 0, "onoff", "ScreenTapDirectionOnOff");
	CLASS_ATTR_SAVE(c,"gestureScreenTapDirectionOnOff", 0);
    /*****************************Gesture info****************************/
    //////gesture attributes
    CLASS_ATTR_FLOAT(c, "screenTapMinDistance", 0, t_MRleap, screenTapMinDistance);
    CLASS_ATTR_LABEL(c, "screenTapMinDistance", 0, "ScreenTapMinDistance");
    CLASS_ATTR_ACCESSORS(c, "screenTapMinDistance", NULL, MRleap_gestureScreenTapMinDistance_set);
    CLASS_ATTR_FILTER_MIN(c, "screenTapMinDistance", 0);
    CLASS_ATTR_SAVE(c,"screenTapMinDistance", 0);
    
    CLASS_ATTR_FLOAT(c, "screenTapHistorySeconds", 0, t_MRleap, screenTapHistorySeconds);
    CLASS_ATTR_LABEL(c, "screenTapHistorySeconds", 0, "ScreenTapHistorySeconds");
    CLASS_ATTR_ACCESSORS(c, "screenTapHistorySeconds", NULL, MRleap_gestureScreenTapHistorySeconds_set);
    CLASS_ATTR_FILTER_MIN(c, "screenTapMinDistance", 0);
    CLASS_ATTR_SAVE(c,"screenTapHistorySeconds", 0);
    
    CLASS_ATTR_FLOAT(c, "screenTapMinForwardVelocity", 0, t_MRleap, screenTapMinForwardVelocity);
    CLASS_ATTR_LABEL(c, "screenTapMinForwardVelocity", 0, "ScreenTapMinForwardVelocity");
    CLASS_ATTR_ACCESSORS(c, "screenTapMinForwardVelocity", NULL, MRleap_gestureScreenTapMinForwardVelocity_set);
    CLASS_ATTR_FILTER_MIN(c, "screenTapHistorySeconds", 0);
    CLASS_ATTR_SAVE(c,"screenTapMinForwardVelocity", 0);
    
    CLASS_ATTR_FLOAT(c, "keyTapMinDistance", 0, t_MRleap, keyTapMinDistance);
    CLASS_ATTR_LABEL(c, "keyTapMinDistance", 0, "KeyTapMinDistance");
    CLASS_ATTR_ACCESSORS(c, "keyTapMinDistance", NULL, MRleap_gestureKeyTapMinDistance_set);
    CLASS_ATTR_FILTER_MIN(c, "keyTapMinDistance", 0);
    CLASS_ATTR_SAVE(c,"keyTapMinDistance", 0);
    
    CLASS_ATTR_FLOAT(c, "keyTapHistorySeconds", 0, t_MRleap, keyTapHistorySeconds);
    CLASS_ATTR_LABEL(c, "keyTapHistorySeconds", 0, "KeyTapHistorySeconds");
    CLASS_ATTR_ACCESSORS(c, "keyTapHistorySeconds", NULL, MRleap_gestureKeyTapHistorySeconds_set);
    CLASS_ATTR_FILTER_MIN(c, "keyTapHistorySeconds", 0);
    CLASS_ATTR_SAVE(c,"keyTapHistorySeconds", 0);
    
    CLASS_ATTR_FLOAT(c, "keyTapMinDownVelocity", 0, t_MRleap, keyTapMinDownVelocity);
    CLASS_ATTR_LABEL(c, "keyTapMinDownVelocity", 0, "KeyTapMinDownVelocity");
    CLASS_ATTR_ACCESSORS(c, "keyTapMinDownVelocity", NULL, MRleap_gestureKeyTapMinDownVelocity_set);
    CLASS_ATTR_FILTER_MIN(c, "keyTapMinDownVelocity", 0);
    CLASS_ATTR_SAVE(c,"keyTapMinDownVelocity", 0);
    
    CLASS_ATTR_FLOAT(c, "swipeMinVelocity", 0, t_MRleap, swipeMinVelocity);
    CLASS_ATTR_LABEL(c, "swipeMinVelocity", 0, "SwipeMinVelocity");
    CLASS_ATTR_ACCESSORS(c, "swipeMinVelocity", NULL, MRleap_gestureSwipeMinVelocity_set);
    CLASS_ATTR_FILTER_MIN(c, "swipeMinVelocity", 0);
    CLASS_ATTR_SAVE(c,"swipeMinVelocity", 0);
    
    CLASS_ATTR_FLOAT(c, "swipeMinLength", 0, t_MRleap, swipeMinLength);
    CLASS_ATTR_LABEL(c, "swipeMinLength", 0, "SwipeMinLength");
    CLASS_ATTR_ACCESSORS(c, "swipeMinLength", NULL, MRleap_gestureSwipeMinLength_set);
    CLASS_ATTR_FILTER_MIN(c, "swipeMinLength", 0);
    CLASS_ATTR_SAVE(c,"swipeMinLength", 0);
    
    CLASS_ATTR_FLOAT(c, "circleMinRadius", 0, t_MRleap, circleMinRadius);
    CLASS_ATTR_LABEL(c, "circleMinRadius", 0, "CircleMinRadius");
    CLASS_ATTR_ACCESSORS(c, "circleMinRadius", NULL, MRleap_gestureCircleMinRadius_set);
    CLASS_ATTR_FILTER_MIN(c, "circleMinRadius", 0);
    CLASS_ATTR_SAVE(c,"circleMinRadius", 0);
    
    CLASS_ATTR_FLOAT(c, "circleMinArc", 0, t_MRleap, circleMinArc);
	CLASS_ATTR_LABEL(c, "circleMinArc", 0, "CircleMinArc");
    CLASS_ATTR_ACCESSORS(c, "circleMinArc", NULL, MRleap_gestureCircleMinArc_set);
    CLASS_ATTR_FILTER_MIN(c, "circleMinArc", 0);
	CLASS_ATTR_SAVE(c,"circleMinArc", 0);
    /*********************************************************************/
    //order of attributes
    CLASS_ATTR_ORDER(c, "frameHist",                            0, "1");
    CLASS_ATTR_ORDER(c, "allDegOnOff",                          0, "2");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "imageOnOff",                           0, "3");
    CLASS_ATTR_ORDER(c, "imageDistortionOnOff",                 0, "4");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "frameMainOnOff",                       0, "5");
    CLASS_ATTR_ORDER(c, "frameRotOnOff",                        0, "6");
    CLASS_ATTR_ORDER(c, "frameRotProb",                         0, "7");
    CLASS_ATTR_ORDER(c, "frameRotRawOnOff",                     0, "8");
    CLASS_ATTR_ORDER(c, "frameScaleOnOff",                      0, "9");
    CLASS_ATTR_ORDER(c, "frameScaleProb",                       0, "10");
    CLASS_ATTR_ORDER(c, "frameScaleRawOnOff",                   0, "11");
    CLASS_ATTR_ORDER(c, "frameTranslationOnOff",                0, "12");
    CLASS_ATTR_ORDER(c, "frameTranslationProb",                 0, "13");
    CLASS_ATTR_ORDER(c, "frameTranslationNormOnOff",            0, "14");
    CLASS_ATTR_ORDER(c, "frameTranslationRawOnOff",             0, "15");
    CLASS_ATTR_ORDER(c, "frameRotationMatrixOnOff",             0, "16");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "handMainOnOff",                        0, "17");
    CLASS_ATTR_ORDER(c, "handSphereOnOff",                      0, "18");
    CLASS_ATTR_ORDER(c, "handSphereNormOnOff",                  0, "19");
    CLASS_ATTR_ORDER(c, "handPositionOnOff",                    0, "20");
    CLASS_ATTR_ORDER(c, "handPositionNormOnOff",                0, "21");
    CLASS_ATTR_ORDER(c, "handPositionStabilizationOnOff",       0, "22");
    CLASS_ATTR_ORDER(c, "handPositionVelocityOnOff",            0, "23");
    CLASS_ATTR_ORDER(c, "handRotationOnOff",                    0, "24");
    CLASS_ATTR_ORDER(c, "handRotationProb",                     0, "25");
    CLASS_ATTR_ORDER(c, "handRotationRawOnOff",                 0, "26");
    CLASS_ATTR_ORDER(c, "handScaleOnOff",                       0, "27");
    CLASS_ATTR_ORDER(c, "handScaleProb",                        0, "28");
    CLASS_ATTR_ORDER(c, "handScaleRawOnOff",                    0, "29");
    CLASS_ATTR_ORDER(c, "handTranslationOnOff",                 0, "30");
    CLASS_ATTR_ORDER(c, "handTranslationProb",                  0, "31");
    CLASS_ATTR_ORDER(c, "handTranslationRawOnOff",              0, "32");
    CLASS_ATTR_ORDER(c, "handTranslationNormOnOff",             0, "33");
    CLASS_ATTR_ORDER(c, "handRotationMatrixOnOff",              0, "34");
    CLASS_ATTR_ORDER(c, "handHorizontalPlaneOnOff",             0, "35");
    //////2.x stuff
    CLASS_ATTR_ORDER(c, "handBasisOnOff",                       0, "36");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "armBasisOnOff",                        0, "37");
    CLASS_ATTR_ORDER(c, "armCenterOnOff",                       0, "38");
    CLASS_ATTR_ORDER(c, "armCenterNormOnOff",                   0, "39");
    CLASS_ATTR_ORDER(c, "armDirectionOnOff",                    0, "40");
    CLASS_ATTR_ORDER(c, "armElbowPositionOnOff",                0, "41");
    CLASS_ATTR_ORDER(c, "armElbowPositionNormOnOff",            0, "42");
    CLASS_ATTR_ORDER(c, "armWristPositionOnOff",                0, "43");
    CLASS_ATTR_ORDER(c, "armWristPositionNormOnOff",            0, "44");
    CLASS_ATTR_ORDER(c, "armWidthOnOff",                        0, "45");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "fingerMainOnOff",                      0, "46");
    CLASS_ATTR_ORDER(c, "fingerDirectionOnOff",                 0, "47");
    CLASS_ATTR_ORDER(c, "fingerTipPositionOnOff",               0, "48");
    CLASS_ATTR_ORDER(c, "fingerTipNormOnOff",                   0, "49");
    CLASS_ATTR_ORDER(c, "fingerTipVelocityOnOff",               0, "50");
    CLASS_ATTR_ORDER(c, "fingerDimensionOnOff",                 0, "51");
    CLASS_ATTR_ORDER(c, "fingerTouchZoneOnOff",                 0, "52");
    ///////2.x stuff
    CLASS_ATTR_ORDER(c, "fingerIsExtendedOnOff",                0, "53");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "toolMainOnOff",                        0, "54");
    CLASS_ATTR_ORDER(c, "toolDirectionOnOff",                   0, "55");
    CLASS_ATTR_ORDER(c, "toolTipPositionOnOff",                 0, "56");
    CLASS_ATTR_ORDER(c, "toolTipNormOnOff",                     0, "57");
    CLASS_ATTR_ORDER(c, "toolTipVelocityOnOff",                 0, "58");
    CLASS_ATTR_ORDER(c, "toolDimensionOnOff",                   0, "59");
    CLASS_ATTR_ORDER(c, "toolTouchZoneOnOff",                   0, "60");
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "gestureCircleOnOff",                   0, "61");
    CLASS_ATTR_ORDER(c, "gestureKeyTapOnOff",                   0, "62");
    CLASS_ATTR_ORDER(c, "gestureScreenTapOnOff",                0, "63");
    CLASS_ATTR_ORDER(c, "gestureSwipeOnOff",                    0, "64");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "gestureCircleMainOnOff",               0, "65");
    CLASS_ATTR_ORDER(c, "gestureCircleCenterOnOff",             0, "66");
    CLASS_ATTR_ORDER(c, "gestureCircleCenterNormOnOff",         0, "67");
    CLASS_ATTR_ORDER(c, "gestureCircleDataOnOff",               0, "68");
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "gestureSwipeMainOnOff",                0, "69");
    CLASS_ATTR_ORDER(c, "gestureSwipePositionOnOff",            0, "70");
    CLASS_ATTR_ORDER(c, "gestureSwipePositionNormOnOff",        0, "71");
    CLASS_ATTR_ORDER(c, "gestureSwipeDirectionOnOff",           0, "72");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "gestureKeyTapMainOnOff",               0, "73");
    CLASS_ATTR_ORDER(c, "gestureKeyTapPositionOnOff",           0, "74");
    CLASS_ATTR_ORDER(c, "gestureKeyTapPositionNormOnOff",       0, "75");
    CLASS_ATTR_ORDER(c, "gestureKeyTapDirectionOnOff",          0, "76");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "gestureScreenTapMainOnOff",            0, "77");
    CLASS_ATTR_ORDER(c, "gestureScreenTapPositionOnOff",        0, "78");
    CLASS_ATTR_ORDER(c, "gestureScreenTapPositionNormOnOff",    0, "79");
    CLASS_ATTR_ORDER(c, "gestureScreenTapDirectionOnOff",       0, "80");
    //////gesture attributes
    CLASS_ATTR_ORDER(c, "circleMinRadius",                      0, "81");
    CLASS_ATTR_ORDER(c, "circleMinArc",                         0, "82");
    CLASS_ATTR_ORDER(c, "swipeMinLength",                       0, "83");
    CLASS_ATTR_ORDER(c, "swipeMinVelocity",                     0, "84");
    CLASS_ATTR_ORDER(c, "keyTapMinDownVelocity",                0, "85");
    CLASS_ATTR_ORDER(c, "keyTapHistorySeconds",                 0, "86");
    CLASS_ATTR_ORDER(c, "keyTapMinDistance",                    0, "87");
    CLASS_ATTR_ORDER(c, "screenTapMinForwardVelocity",          0, "88");
    CLASS_ATTR_ORDER(c, "screenTapHistorySeconds",              0, "89");
    CLASS_ATTR_ORDER(c, "screenTapMinDistance",                 0, "90");
    /*********************************************************************/
    
	class_register(CLASS_BOX, c);
    
	MRleap_class = c;
    
	return 0;
}
/************************************/
void *MRleap_new(t_symbol *s, long argc, t_atom *argv)
{
	t_MRleap *x = NULL;
    
	x = (t_MRleap *)object_alloc((t_class *)MRleap_class);
    
    object_post((t_object *)x, "MRleap 0.4.1 for The Leap 2.1");
    
    x->prevFrameID                      = 0;
    x->frameHist                        = 1;
    x->allDegOnOff                      = 0; //radians by default
    ////////image
    x->imageOnOff                       = false;
    x->imageDistortionOnOff             = false;
    ////////frame
    x->frameRotProb                     = 0.4;
    x->frameMainOnOff                   = false;
    x->frameRotOnOff                    = false;
    x->frameRotRawOnOff                 = false;
    x->frameScaleOnOff                  = false;
    x->frameScaleProb                   = 0.4;
    x->frameScaleRawOnOff               = false;
    x->frameTranslationProb             = 0.4;
    x->frameTranslationOnOff            = false;
    x->frameTranslationRawOnOff         = false;
    x->frameTranslationNormOnOff        = false;
    x->frameRotationMatrix              = false;
    ///////////hand
    x->handMainOnOff                    = false;
    x->handSphereOnOff                  = false;
    x->handSphereNorm                   = false;
    x->handPositionOnOff                = false;
    x->handPositionNormOnOff            = false;
    x->handPositionStabilizationOnOff   = false;
    x->handRotationProb                 = 0.4;
    x->handRotationOnOff                = false;
    x->handRotationRawOnOff             = false;
    x->handScaleRawOnOff                = false;
    x->handScaleOnOff                   = false;
    x->handScaleProb                    = 0.4;
    x->handTranslationRawOnOff          = false;
    x->handTranslationNormOnOff         = false;
    x->handTranslationOnOff             = false;
    x->handTranslationProb              = 0.4;
    x->handRotationMatrix               = false;
    x->handHorizontalPlaneOnOff         = false;
    x->handPositionVelocityOnOff        = false;
    ////2.x stuff
    x->handBasisOnOff                   = false;
    /////
    //////Arm
    x->armBasisOnOff                    = false;
    x->armCenterOnOff                   = false;
    x->armDirectionOnOff                = false;
    x->armElbowPositionOnOff            = false;
    x->armWristPositionOnOff            = false;
    x->armWidthOnOff                    = false;
    //////////pointable
    x->toolMainOnOff                    = false;
    x->fingerMainOnOff                  = false;
    x->toolDirectionOnOff               = false;
    x->fingerDirectionOnOff             = false;
    x->toolTipPositionOnOff             = false;
    x->toolTipNormOnOff                 = false;
    x->fingerTipPositionOnOff           = false;
    x->fingerTipNormOnOff               = false;
    x->toolTipVelocityOnOff             = false;
    x->fingerTipVelocityOnOff           = false;
    x->toolDimensionOnOff               = false;
    x->fingerDimensionOnOff             = false;
    x->toolTouchZoneOnOff               = false;
    x->fingerTouchZoneOnOff             = false;
    /////////fingers 2.x
    x->fingerIsExtendedOnOff            = false;
    //////////gestures
    x->gestureCircleMainOnOff           = false;
    x->gestureCircleCenterOnOff         = false;
    x->gestureCircleCenterNormOnOff     = false;
    x->gestureCircleDataOnOff           = false;
    ///-----------------------------------------
    x->gestureSwipeMainOnOff            = false;
    x->gestureSwipePositionOnOff        = false;
    x->gestureSwipePositionNormOnOff    = false;
    x->gestureSwipeDirectionOnOff       = false;
    //-----------------------------------------
    x->gestureKeyTapMainOnOff           = false;
    x->gestureKeyTapPositionOnOff       = false;
    x->gestureKeyTapPositionNormOnOff   = false;
    x->gestureKeyTapDirectionOnOff      = false;
    //-----------------------------------------
    x->gestureScreenTapMainOnOff        = false;
    x->gestureScreenTapPositionOnOff    = false;
    x->gestureScreenTapPositionNormOnOff= false;
    x->gestureScreenTapDirectionOnOff   = false;
    //default gesture values
    x->circleMinRadiusDef              = 5.;
    x->circleMinArcDef                 = 1.5 * Leap::PI;
    x->swipeMinLengthDef               = 150;
    x->swipeMinVelocityDef             = 1000;
    x->keyTapMinDownVelocityDef        = 50;
    x->keyTapHistorySecondsDef         = 0.1;
    x->keyTapMinDistanceDef            = 3;
    x->screenTapMinForwardVelocityDef  = 50;
    x->screenTapHistorySecondsDef      = 0.1;
    x->screenTapMinDistanceDef         = 5;
    
    
    x->circleMinRadius                  = x->circleMinRadiusDef;
    x->circleMinArc                     = x->circleMinArcDef;
    x->swipeMinLength                   = x->swipeMinLengthDef;
    x->swipeMinVelocity                 = x->swipeMinVelocityDef;
    x->keyTapMinDownVelocity            = x->keyTapMinDownVelocityDef;
    x->keyTapHistorySeconds             = x->keyTapHistorySecondsDef;
    x->keyTapMinDistance                = x->keyTapMinDistanceDef;
    x->screenTapMinForwardVelocity      = x->screenTapMinForwardVelocityDef;
    x->screenTapHistorySeconds          = x->screenTapHistorySecondsDef;
    x->screenTapMinDistance             = x->screenTapMinDistanceDef;
    
    /////////outlets
    x->matrix_nameLeft  = gensym("leapCameraLeft");
    x->matrix_nameRight = gensym("leapCameraRight");
    x->plane            = 0;
    x->offsetcount      = 0;
    x->prevDim[0]       = 0;
    x->prevDim[1]       = 0;
    
    
    x->outletStart      = outlet_new((t_MRleap *)x, NULL);
    x->outletFrame      = outlet_new((t_MRleap *)x, NULL);
    x->outletHands      = outlet_new((t_MRleap *)x, NULL);
    x->outletArms       = outlet_new((t_MRleap *)x, NULL);
    x->outletTools      = outlet_new((t_MRleap *)x, NULL);
    x->outletFingers    = outlet_new((t_MRleap *)x, NULL);
    x->outletGestures   = outlet_new((t_MRleap *)x, NULL);
    
    
    
    // Create a controller
    x->leap = new Leap::Controller;
    
    
    ///////genSyms
    
    x->s_frameMain              = gensym("frameMain");
    x->s_x                      = gensym("x");
    x->s_y                      = gensym("y");
    x->s_z                      = gensym("z");
    x->s_rotation               = gensym("rotation");
    x->s_rotationRaw            = gensym("rotationRaw");
    x->s_scale                  = gensym("scale");
    x->s_scaleRaw               = gensym("scaleRaw");
    x->s_translation            = gensym("translation");
    x->s_translationRaw         = gensym("translationRaw");
    x->s_rotationMatrix         = gensym("rotationMatrix");
    x->s_handMain               = gensym("handMain");
    x->s_position               = gensym("position");
    x->s_sphere                 = gensym("sphere");
    x->s_plane                  = gensym("plane");
    x->s_toolMain               = gensym("toolMain");
    x->s_fingerMain             = gensym("fingerMain");
    x->s_direction              = gensym("direction");
    x->s_velocity               = gensym("velocity");
    x->s_basis                  = gensym("basis");
    x->s_center                 = gensym("center");
    x->s_elbowPosition          = gensym("elbowPosition");
    x->s_wristPosition          = gensym("wristPosition");
    x->s_width                  = gensym("width");
    x->s_extended               = gensym("extended");
    
    x->s_tip                    = gensym("tip");
    x->s_tipVelocity            = gensym("tipVelocity");
    x->s_tipDimension           = gensym("tipDimension");
    x->s_touchZone              = gensym("touchZone");
    x->s_circleMain             = gensym("circleMain");
    x->s_circleCenter           = gensym("circleCenter");
    x->s_circleData             = gensym("circleData");
    x->s_keyTapMain             = gensym("keyTapMain");
    x->s_keyTapPosition         = gensym("keyTapPosition");
    x->s_keyTapDirection        = gensym("keyTapDirection");
    x->s_screenTapMain          = gensym("screenTapMain");
    x->s_screenTapPosition      = gensym("screenTapPosition");
    x->s_screenTapDirection     = gensym("screenTapDirection");
    x->s_swipeMain              = gensym("swipeMain");
    x->s_swipePosition          = gensym("swipePosition");
    x->s_swipeDirection         = gensym("swipeDirection");
    
    
    x->fingerNames[0]           = gensym("Thumb");
    x->fingerNames[1]           = gensym("Index");
    x->fingerNames[2]           = gensym("Middle");
    x->fingerNames[3]           = gensym("Ring");
    x->fingerNames[4]           = gensym("Pinky");
    
    x->RIGHT = gensym("right");
    x->LEFT  = gensym("left");
    x->HAND  = x->LEFT;
    x->OTHER = gensym("other");
    
    // call this after initializing defaults to overwrite with saved attr
    attr_args_process(x, argc, argv);
    
    //    Leap::Device testDevice;
    
    x->m_clock = clock_new((t_MRleap *) x, (method)MRleap_checkLeapConnection);
    
    clock_fdelay(x->m_clock, 1000);
    
    ////////gesture attr default
    //////////gesture attr
    
//    MRleap_gestureResetAll(x);
    
	return (x);
}
/************************************/
void MRleap_checkLeapConnection(t_MRleap *x)
{
    if (x->leap->isConnected()) {
        
        post_sym(x, gensym("Leap found and connected..."));
        x->leap->setPolicyFlags(Leap::Controller::POLICY_IMAGES);
    }
    else {
        
        object_error((t_object *)x, "No Leap found or unable to connect...");
        object_error((t_object *)x, "reattempting to connect on bang...");
    }
}
/************************************/
void MRleap_assist(t_MRleap *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
		sprintf(s, "bang get next frame");
	}
	else {	// outlet
        
        switch (a) {
            case 0:
                sprintf(s, "Gestures");
                break;
                
            case 1:
                sprintf(s, "Fingers");
                break;
            case 2:
                sprintf(s, "Tools");
                break;
            case 3:
                sprintf(s, "Arms");
                break;
            case 4:
                sprintf(s, "Hands");
                break;
            case 5:
                sprintf(s, "Frame");
                break;
            case 6:
                sprintf(s, "frame start/end [1/0]");
                break;
        }
	}
}
/************************************/
void MRleap_free(t_MRleap *x)
{
    if (x->leap)    {
		delete (Leap::Controller *)(x->leap);
    }
    object_free(x->m_clock);
}
/************************************/
void MRleap_bang(t_MRleap *x)
{
//    Leap::Image image;
    
//    MRleap_getDistortion(x, image);
    
    
    
    
    
    if( x->leap->isConnected()) //controller is a Controller object
    {
        
        const Leap::Frame frame = x->leap->frame();
        
        if (frame.isValid())    {
            
            x->curFrameID   = frame.id();
            
            // ignore the same frame
            if (x->curFrameID == x->prevFrameID) {
                
                return;
            }
            
            /*************Image info****************/
            MRleap_getImageData(x, frame);
            
            /**************************************/
            //first get the image data, then bang for start of frame ->
            //can use the bang to bang the matrix in the patch
            outlet_int(x->outletStart, (long)1);
            
            /*************Frame info****************/
            MRleap_getFrameData(x, frame);
            
            
            /***************Hand info****************/
            if (!frame.hands().isEmpty()) {
                
                //Process hands...
                MRleap_getHandData(x, frame);
                
                MRleap_getArmData(x, frame);
            }
            /***************Pointables info****************/
            if (!frame.pointables().isEmpty()) {
                
                //Process pointables
                MRleap_getToolData(x, frame);
                MRleap_getFingerData(x, frame);
            }
            /***************Gesture info*******************/
            if (!frame.gestures().isEmpty())    {

                MRleap_getGestureData(x, frame);
            }
            x->prevFrameID = x->curFrameID;
        
            outlet_int(x->outletStart, (long)0);
        }
    }
    else if (!x->leap->isConnected())    {
        //try to connect again if connection failed
    
        jit_error_sym(x, gensym("Trying to connect to Leap..."));
        
        MRleap_checkLeapConnection(x);
    }
}
/************************************/
void MRleap_getFrameData(t_MRleap *x, Leap::Frame frame)
{
    
    const Leap::HandList    hands       = frame.hands();
    const int               numHands    = hands.count(); //what's the max?? (have seen up to 5 so far)
    float                   fps         = frame.currentFramesPerSecond();
    
    if(x->frameMainOnOff)    {
        
      
        /***************Frame info*********************/
        //output frame info orientation translation etc....
        
        t_atom frameMain[4];
        atom_setlong(frameMain,     frame.id());
        atom_setlong(frameMain+1,   numHands);
        atom_setlong(frameMain+2,   frame.timestamp() * 0.001); //microsec -> ms
        atom_setfloat(frameMain+3,  fps);
        
        outlet_anything(x->outletFrame, x->s_frameMain, 4, frameMain);
        /////////////////////////////////////////////
    }
    
    ///////////////frame rotation//////////////////
    ////////////////////////////////
    float rotationProb = frame.rotationProbability(x->leap->frame(x->frameHist));
    
    if (rotationProb >= x->frameRotProb && x->frameRotOnOff == true)  {
        
        t_atom frameRotCooked[4];
        
        float whichAxis = MRleap_biggestRotAxis(x, frame.rotationAxis(x->leap->frame(x->frameHist)));
        float rotationAngle  = -1;
        
        if (whichAxis == X_AXIS)    {
            
            atom_setsym(frameRotCooked+2,          x->s_x);
            rotationAngle = frame.rotationAngle(x->leap->frame(x->frameHist), Leap::Vector::xAxis());
        }
        else if (whichAxis == Y_AXIS)    {
            
            atom_setsym(frameRotCooked+2,          x->s_y);
            rotationAngle = frame.rotationAngle(x->leap->frame(x->frameHist), Leap::Vector::yAxis());
        }
        else if (whichAxis == Z_AXIS)    {
            
            atom_setsym(frameRotCooked+2,          x->s_z);
            rotationAngle = frame.rotationAngle(x->leap->frame(x->frameHist), Leap::Vector::zAxis());
        }
        
        rotationAngle = rotationAngle * MRleap_RadDeg(x);
        
        atom_setlong(frameRotCooked,         frame.id());
        atom_setfloat(frameRotCooked+1,      rotationProb);
        atom_setfloat(frameRotCooked+3,      rotationAngle);
        
        outlet_anything(x->outletFrame, x->s_rotation, 4, frameRotCooked);
    }
    /////////////////////////////////
    if (x->frameRotRawOnOff)    {
        
        t_atom frameRotRaw[8];
        atom_setlong(frameRotRaw,         frame.id());
        atom_setfloat(frameRotRaw+1,      rotationProb);
        atom_setfloat(frameRotRaw+2,      frame.rotationAxis(x->leap->frame(x->frameHist))[0]);
        atom_setfloat(frameRotRaw+3,      frame.rotationAxis(x->leap->frame(x->frameHist))[1]);
        atom_setfloat(frameRotRaw+4,      frame.rotationAxis(x->leap->frame(x->frameHist))[2]);
        atom_setfloat(frameRotRaw+5,      frame.rotationAngle(x->leap->frame(x->frameHist), Leap::Vector::xAxis()) * MRleap_RadDeg(x));
        atom_setfloat(frameRotRaw+6,      frame.rotationAngle(x->leap->frame(x->frameHist), Leap::Vector::yAxis()) * MRleap_RadDeg(x));
        atom_setfloat(frameRotRaw+7,      frame.rotationAngle(x->leap->frame(x->frameHist), Leap::Vector::zAxis()) * MRleap_RadDeg(x));
        
        
        outlet_anything(x->outletFrame, x->s_rotationRaw, 8, frameRotRaw);
        
    }
    /////////////scale/////////////////////
    float scaleProb = frame.scaleProbability(x->leap->frame(x->frameHist));
    
    if (x->frameScaleRawOnOff)  {
        
        t_atom frameScaleRaw[3];
        
        atom_setlong (frameScaleRaw,        frame.id());
        atom_setfloat(frameScaleRaw+1,      scaleProb);
        atom_setfloat(frameScaleRaw+2,      frame.scaleFactor(x->leap->frame(x->frameHist)));
        
        outlet_anything(x->outletFrame, x->s_scaleRaw, 3, frameScaleRaw);
    }
    
    ///////////////////////////////////
    if (x->frameScaleOnOff && x->frameScaleProb > scaleProb)  {
        
        t_atom frameScale[3];
        
        atom_setlong (frameScale,       frame.id());
        atom_setfloat(frameScale+1,     scaleProb);
        atom_setfloat(frameScale+2,     frame.scaleFactor(x->leap->frame(x->frameHist)));
        
        outlet_anything(x->outletFrame, x->s_scale, 3, frameScale);
    }
    ////////////////////////////////
    
    /////////////translation/////////////////////
    float translationProb = frame.translationProbability(x->leap->frame(x->frameHist));
    
    if (x->frameTranslationRawOnOff)    {
        
        Leap::Vector trans = Leap::Vector::zero();
        
        trans = MRleap_normalizeVec(x, frame, frame.translation(x->leap->frame(x->frameHist)), x->frameTranslationNormOnOff);
        
        
        t_atom frameTranslationRaw[5];
        
        atom_setlong (frameTranslationRaw,      frame.id());
        atom_setfloat(frameTranslationRaw+1,    translationProb);
        atom_setfloat(frameTranslationRaw+2,    trans.x);
        atom_setfloat(frameTranslationRaw+3,    trans.y);
        atom_setfloat(frameTranslationRaw+4,    trans.z);
        
        outlet_anything(x->outletFrame, x->s_translationRaw, 5, frameTranslationRaw);
    }
    
    if (x->frameTranslationOnOff && x->frameTranslationProb > translationProb)  {
        
        Leap::Vector trans = Leap::Vector::zero();
        
        trans = MRleap_normalizeVec(x, frame, frame.translation(x->leap->frame(x->frameHist)), x->frameTranslationNormOnOff);
        
        t_atom frameTranslation[5];
        
        atom_setlong (frameTranslation,     frame.id());
        atom_setfloat(frameTranslation+1,   translationProb);
        atom_setfloat(frameTranslation+2,   trans.x);
        atom_setfloat(frameTranslation+3,   trans.y);
        atom_setfloat(frameTranslation+4,   trans.z);
        
        outlet_anything(x->outletFrame, x->s_translation, 5, frameTranslation);
    }
    ///////////////////////////////////////////////
    
    ///////////////matrix/////////////////////
    
    if (x->frameRotationMatrix) {
        
        Leap::FloatArray array = frame.rotationMatrix(x->leap->frame(x->frameHist)).toArray3x3();
        t_atom frameRotationMatrix[10];
        
        atom_setlong (frameRotationMatrix,     frame.id());
        
        for (int i = 1; i < 10; i++)    {
            
            atom_setfloat(frameRotationMatrix + i, array.m_array[i - 1]);
        }
        
        outlet_anything(x->outletFrame, x->s_rotationMatrix, 10, frameRotationMatrix);
    }
}
/************************************/
void MRleap_getImageData(t_MRleap *x, Leap::Frame frame)
{
    
    if (x->imageOnOff)  {
        

        Leap::ImageList images = frame.images();
        //two cameras in the leap; index 0 = left camera; index 1 = right (when looking at it from the front, green light facing user
        Leap::Image image   = images[0];
        Leap::Image image2  = images[1];
        
        if (image.isValid() && image2.isValid()) {
           
            
            MRleap_getImage(x, image, x->matrix_nameLeft);
            
            MRleap_getImage(x, image2, x->matrix_nameRight);
            
//            MRleap_getDistortion(x, image);
        }
        else    {
            jit_error_sym(x, gensym("No valid images available from leap"));
        }
    }
}
/************************************/
void MRleap_getDistortion(t_MRleap *x, Leap::Image image)
{
    void *matrixDis;
    long i = 0,j = 0;
    long savelock = 0;
    t_jit_matrix_info minfo;
    char *outM = 0, *pD, *out_data;
    
     t_atom val[JIT_MATRIX_MAX_PLANECOUNT];
    
    post("HELLO");
    const float *distortion_buffer = image.distortion();
    
    long buffer_size = 200; //image.distortionWidth() * image.distortionHeight() * 4;
    
    
    
    matrixDis = jit_object_findregistered(gensym("distortionMap"));
    
    if (matrixDis && jit_object_method(matrixDis, _jit_sym_class_jit_matrix)) {
        
        
        savelock  = (long) jit_object_method(matrixDis,_jit_sym_lock, 1);
        jit_object_method(matrixDis,_jit_sym_getinfo,&minfo);
        jit_object_method(matrixDis,_jit_sym_getdata,&outM);
        
        
        if ((!outM)||(x->plane >= minfo.planecount)||(x->plane < 0)) {
            
            jit_error_sym(x, _jit_sym_err_calculate);
            jit_object_method(matrixDis, _jit_sym_lock, savelock);
            goto out;
        }
        
        minfo.type          = _jit_sym_char;
        minfo.dimcount      = 2;
        minfo.planecount    = 2;
        minfo.dim[0]        = 5; //image.distortionWidth() * 0.5;
        minfo.dim[1]        = 5; //image.distortionHeight();
        
        jit_object_method(matrixDis, _jit_sym_setinfo, &minfo);
        jit_object_method(matrixDis,_jit_sym_getinfo,&minfo);
        //update everything with new info (so dimstride is calculated)
        
        
        //jit_parallel_ndim_simplecalc2(<#method fn#>, <#void *data#>, <#long dimcount#>, <#long *dim#>, <#long planecount#>, <#t_jit_matrix_info *minfo1#>, <#char *bp1#>, <#t_jit_matrix_info *minfo2#>, <#char *bp2#>, <#long flags1#>, <#long flags2#>)

       
        j = minfo.dim[0];
        
        outM  += minfo.planecount;
    
        
        for (i = 0; i < 25; i++, j++) {

            pD   = outM + (j / minfo.dim[0]) * minfo.dimstride[1] + (j % minfo.dim[0]) * minfo.dimstride[0] - minfo.dimstride[1] - minfo.dimstride[0];
            
            *pD++     = i + 100;
            *pD++     = j + 150;
           
        }
    
        jit_object_method(matrixDis,_jit_sym_lock,savelock);
 
    }
    else {
 
        jit_error_sym(x,_jit_sym_err_calculate);
        goto out;
    }
 
out:
 
    return;
}
/************************************/
void MRleap_getImage(t_MRleap *x, Leap::Image image, t_symbol *matrixName)
{
    void *matrix;
    long i,j;
    long savelock = 0, offset0, offset1;
    t_jit_matrix_info minfo;
    char *bp, *p;
    
    
    //find matrix
    matrix = jit_object_findregistered(matrixName);
    
    if (matrix && jit_object_method(matrix, _jit_sym_class_jit_matrix)) {
        
        
        savelock  = (long) jit_object_method(matrix,_jit_sym_lock, 1);
            jit_object_method(matrix,_jit_sym_getinfo,&minfo);
        jit_object_method(matrix,_jit_sym_getdata,&bp);
        
        
        if ((!bp)||(x->plane >= minfo.planecount)||(x->plane < 0)) {

            jit_error_sym(x, _jit_sym_err_calculate);
            jit_object_method(matrix, _jit_sym_lock, savelock);
            goto out;
        }
    
        minfo.type          = _jit_sym_char;
        minfo.dimcount      = 2;
        minfo.planecount    = 2;
        minfo.dim[0]        = image.width();
        minfo.dim[1]        = image.height();
            
        jit_object_method(matrix, _jit_sym_setinfo, &minfo);
            
//        x->prevDim[0] = image.width();
//        x->prevDim[1] = image.height();


        x->imageBufferLength = image.width() * image.height();
        
//        post("width = %d   height = %d   length = %d    ", image.width(), image.height(), imageBufferLength);
        
//        const unsigned char* image_buffer   = image.data();
        
        
        j = minfo.dim[0];
        
        bp += minfo.planecount;
        
        for (i = 0; i < x->imageBufferLength; i++, j++) {
            
            p   = bp + (j / minfo.dim[0]) * minfo.dimstride[1] + (j % minfo.dim[0]) * minfo.dimstride[0];
            
             if (x->imageDistortionOnOff) {

                    //NOT IMPLEMENTED YET
 
                *((uchar *)p)   = abs(image.data()[i] -255);
             }
            
             else {
                 
             
                *((uchar *)p)   = image.data()[i];
             }
        }
        
        jit_object_method(matrix,_jit_sym_lock,savelock);
    }
    else {

        jit_error_sym(x,_jit_sym_err_calculate);
        goto out;
    }
    
out:
    
    return;
}
/************************************/
void MRleap_getHandData(t_MRleap *x, Leap::Frame frame)
{
    
    Leap::HandList          hands       = frame.hands();
    const int               numHands    = hands.count(); //what's the max?? (have seen up to 5 so far
    
    for(int i = 0; i < numHands; ++i)  {
        
        const Leap::Hand hand = hands[i];
        
        if (hand.isValid()) {
            
            MRleap_assignHandID(x, hand);
            
            long handID             = hand.id();
            float handConfidence    = hand.confidence();
            
            if(x->handMainOnOff)    {
          
                t_atom handMain[9];
                
                atom_setsym(handMain,       x->s_handMain);
                atom_setlong(handMain+1,    handID);
                atom_setlong(handMain+2,    x->curFrameID);
                atom_setfloat(handMain+3,   handConfidence);
                atom_setlong(handMain+4,    hand.timeVisible() * 1000); //sec -> ms
                atom_setfloat(handMain+5,   hand.grabStrength());
                atom_setfloat(handMain+6,   hand.pinchStrength());
                atom_setfloat(handMain+7,   hand.palmWidth()); //in mm
                atom_setlong(handMain+8,    hand.tools().count());
                
                outlet_anything(x->outletHands, x->HAND, 9, handMain);
            }
            ////////////////position//////////
            if (x->handPositionOnOff) {
                
                Leap::Vector pos = Leap::Vector::zero();
                
                if (x->handPositionStabilizationOnOff)  {
                    
                    pos = hand.stabilizedPalmPosition();
                }
                else    {
                    
                    pos = hand.palmPosition();
                }
                
                pos = MRleap_normalizeVec(x, frame, pos, x->handPositionNormOnOff);

                t_atom handPos[7];
 
                atom_setsym(handPos,        x->s_position);
                atom_setlong(handPos+1,     handID);
                atom_setlong(handPos+2,     x->curFrameID);
                atom_setfloat(handPos+3,    handConfidence);
                atom_setfloat(handPos+4,    pos[0]);
                atom_setfloat(handPos+5,    pos[1]);
                atom_setfloat(handPos+6,    pos[2]);
                
                outlet_anything(x->outletHands, x->HAND, 7, handPos);
              
            }
            //////////////////////////////
            
            if (x->handPositionVelocityOnOff)   {
         
                t_atom handVel[7];
                
                atom_setsym(handVel,        x->s_velocity);
                atom_setlong(handVel+1,     handID);
                atom_setlong(handVel+2,     x->curFrameID);
                atom_setfloat(handVel+3,    handConfidence);
                atom_setfloat(handVel+4,    hand.palmVelocity()[0]);
                atom_setfloat(handVel+5,    hand.palmVelocity()[1]);
                atom_setfloat(handVel+6,    hand.palmVelocity()[2]);
            
                outlet_anything(x->outletHands, x->HAND, 7, handVel);
            }
            
            ////////sphere////////////////
            if (x->handSphereOnOff) {
                
                Leap::Vector sphere = MRleap_normalizeVec(x, frame, hand.sphereCenter(), x->handSphereNorm);
                
                t_atom handSphere[8];
                
                atom_setsym(handSphere,         x->s_sphere);
                atom_setlong(handSphere+1,      handID);
                atom_setlong(handSphere+2,      x->curFrameID);
                atom_setfloat(handSphere+3,     handConfidence);
                atom_setfloat(handSphere+4,     hand.sphereRadius());
                atom_setfloat(handSphere+5,     sphere[0]);
                atom_setfloat(handSphere+6,     sphere[1]);
                atom_setfloat(handSphere+7,     sphere[2]);
                
                outlet_anything(x->outletHands, x->HAND, 8, handSphere);
            }
            ///////////rotation////////////////////
            float rotationProb = hand.rotationProbability(x->leap->frame(x->frameHist));
            
            if (rotationProb >= x->handRotationProb && x->handRotationOnOff == true)  {
                
                t_atom handRotCooked[7];
                
                atom_setsym(handRotCooked,       x->s_rotation);
                atom_setlong(handRotCooked+1,    handID);
                atom_setlong(handRotCooked+2,    x->curFrameID);
                atom_setfloat(handRotCooked+3,     handConfidence);
                
                float whichAxis = MRleap_biggestRotAxis(x, hand.rotationAxis(x->leap->frame(x->frameHist)));
                float rotationAngle  = -1;
                
                if (whichAxis == X_AXIS)    {
                    
                    atom_setsym(handRotCooked+5,          x->s_x);
                    rotationAngle = hand.rotationAngle(x->leap->frame(x->frameHist), Leap::Vector::xAxis());
                }
                else if (whichAxis == Y_AXIS)    {
                    
                    atom_setsym(handRotCooked+5,          x->s_y);
                    rotationAngle = hand.rotationAngle(x->leap->frame(x->frameHist), Leap::Vector::yAxis());
                }
                else if (whichAxis == Z_AXIS)    {
                    
                    atom_setsym(handRotCooked+5,          x->s_z);
                    rotationAngle = hand.rotationAngle(x->leap->frame(x->frameHist), Leap::Vector::zAxis());
                }
                
                rotationAngle = rotationAngle * MRleap_RadDeg(x);
                
                atom_setfloat(handRotCooked+4,        rotationProb);
                atom_setfloat(handRotCooked+6,        rotationAngle);
                
                outlet_anything(x->outletHands, x->HAND, 7, handRotCooked);
            }
            
            /////////////////////////////////
            if (x->handRotationRawOnOff)    {
                
                t_atom handRotRaw[11];
                
                atom_setsym(handRotRaw,          x->s_rotationRaw);
                atom_setlong(handRotRaw+1,       handID);
                atom_setlong(handRotRaw+2,       x->curFrameID);
                atom_setfloat(handRotRaw+3,      handConfidence);
                atom_setfloat(handRotRaw+4,      rotationProb);
                atom_setfloat(handRotRaw+5,      hand.rotationAxis(x->leap->frame(1))[0]);
                atom_setfloat(handRotRaw+6,      hand.rotationAxis(x->leap->frame(1))[1]);
                atom_setfloat(handRotRaw+7,      hand.rotationAxis(x->leap->frame(1))[2]);
                atom_setfloat(handRotRaw+8,      hand.rotationAngle(x->leap->frame(1), Leap::Vector::xAxis()) * MRleap_RadDeg(x));
                atom_setfloat(handRotRaw+9,      hand.rotationAngle(x->leap->frame(1), Leap::Vector::yAxis()) * MRleap_RadDeg(x));
                atom_setfloat(handRotRaw+10,     hand.rotationAngle(x->leap->frame(1), Leap::Vector::zAxis()) * MRleap_RadDeg(x));
                
                
                outlet_anything(x->outletHands, x->HAND, 11, handRotRaw);
            }
            /////////////scale/////////////////////
            float scaleProb = hand.scaleProbability(x->leap->frame(x->frameHist));
            
            if (x->handScaleRawOnOff)  {
                
                t_atom handScaleRaw[6];
                
                atom_setsym(handScaleRaw,           x->s_scaleRaw);
                atom_setlong(handScaleRaw+1,        handID);
                atom_setlong(handScaleRaw+2,        x->curFrameID);
                atom_setfloat(handScaleRaw+3,       handConfidence);
                atom_setfloat(handScaleRaw+4,       scaleProb);
                atom_setfloat(handScaleRaw+5,       hand.scaleFactor(x->leap->frame(x->frameHist)));
                
                outlet_anything(x->outletHands, x->HAND, 6, handScaleRaw);
            }
            
            ///////////////////////////////////
            if (x->handScaleOnOff && scaleProb >= x->handScaleProb)  {
                
                t_atom handScale[6];
                
                atom_setsym(handScale,      x->s_scale);
                atom_setlong(handScale+1,   handID);
                atom_setlong(handScale+2,   x->curFrameID);
                atom_setfloat(handScale+3,    handConfidence);
                atom_setfloat(handScale+4,  scaleProb);
                atom_setfloat(handScale+5,  hand.scaleFactor(x->leap->frame(x->frameHist)));
                
                outlet_anything(x->outletHands, x->HAND, 6, handScale);
            }
            ////////////////////////////////
            
            /////////////translation/////////////////////
            float translationProb = hand.translationProbability(x->leap->frame(x->frameHist));
            
            
            
            if (x->handTranslationRawOnOff)    {
                
                Leap::Vector trans = MRleap_normalizeVec(x, frame, hand.translation(x->leap->frame(x->frameHist)), x->handTranslationNormOnOff);
                
                t_atom handTranslationRaw[8];
                
                atom_setsym(handTranslationRaw,         x->s_translationRaw);
                atom_setlong(handTranslationRaw+1,      handID);
                atom_setlong(handTranslationRaw+2,      x->curFrameID);
                atom_setfloat(handTranslationRaw+3,     handConfidence);
                atom_setfloat(handTranslationRaw+4,     translationProb);
                atom_setfloat(handTranslationRaw+5,     trans.x);
                atom_setfloat(handTranslationRaw+6,     trans.y);
                atom_setfloat(handTranslationRaw+7,     trans.z);
                
                outlet_anything(x->outletHands, x->HAND, 8, handTranslationRaw);
            }
            
            if (x->handTranslationOnOff && x->handTranslationProb > translationProb)  {
                
                Leap::Vector trans = MRleap_normalizeVec(x, frame, hand.translation(x->leap->frame(x->frameHist)), x->handTranslationNormOnOff);
                t_atom handTranslation[8];
                
                atom_setsym(handTranslation,        x->s_translation);
                atom_setlong(handTranslation+1,     handID);
                atom_setlong(handTranslation+2,     x->curFrameID);
                atom_setfloat(handTranslation+3 ,   handConfidence);
                atom_setfloat(handTranslation+4,    translationProb);
                atom_setfloat(handTranslation+5,    trans.x);
                atom_setfloat(handTranslation+6,    trans.y);
                atom_setfloat(handTranslation+7,    trans.z);
                
                outlet_anything(x->outletHands, x->HAND, 8, handTranslation);
            }
            ///////////////////////////////////////////////
            ///////////////matrix/////////////////////
            
            if (x->handRotationMatrix) {
                
                Leap::FloatArray array = hand.rotationMatrix(x->leap->frame(x->frameHist)).toArray3x3();
                t_atom handRotationMatrix[13];
                
                atom_setsym(handRotationMatrix,        x->s_rotationMatrix);
                atom_setlong(handRotationMatrix+1,     handID);
                atom_setlong(handRotationMatrix+2,     x->curFrameID);
                atom_setfloat(handRotationMatrix+3,    handConfidence);
                
                for (int i = 4; i < 13; i++)    {
                    
                    atom_setfloat(handRotationMatrix + i, array.m_array[i - 4]);
                }
                
                outlet_anything(x->outletHands, x->HAND, 13, handRotationMatrix);
            }
            
            //////////////horizontal plane////////////
            if (x->handHorizontalPlaneOnOff)    {
                
                t_atom handPlane[7];
                
                atom_setsym(handPlane,          x->s_plane);
                atom_setlong(handPlane+1,       handID);
                atom_setlong(handPlane+2,       x->curFrameID);
                atom_setfloat(handPlane+3,      handConfidence);
                atom_setfloat(handPlane+4,      hand.direction().pitch() * MRleap_RadDeg(x));
                atom_setfloat(handPlane+5,      hand.direction().yaw()   * MRleap_RadDeg(x));
                atom_setfloat(handPlane+6,      hand.palmNormal().roll() * MRleap_RadDeg(x));
                
                outlet_anything(x->outletHands, x->HAND, 7, handPlane);
            }
            
            /************************************/
            
            if (x->handBasisOnOff) {
                
                Leap::FloatArray array = hand.basis().toArray3x3();
                
                t_atom handBasis[13];
                
                atom_setsym(handBasis,          x->s_basis);
                atom_setlong(handBasis+1,       handID);
                atom_setlong(handBasis+2,       x->curFrameID);
                atom_setfloat(handBasis+3,      handConfidence);
                
                for (int i = 4; i < 13; i++)    {
                    
                    atom_setfloat(handBasis + i, array.m_array[i - 4]);
                }

                
                outlet_anything(x->outletHands, x->HAND, 13, handBasis);
            }
        }
    }
}
/************************************/
void MRleap_getArmData(t_MRleap *x, Leap::Frame frame)
{
    
    Leap::HandList          hands       = frame.hands();
    const int               numHands    = hands.count(); //what's the max?? (have seen up to 5 so far
    
    for(int i = 0; i < numHands; ++i)  {
    
        Leap::Arm arm = hands[i].arm();
        Leap::Hand  hand = hands[i];

       if (arm.isValid())  {
        
            MRleap_assignHandID(x, hand);
            long handID             = hand.id();

            if (x->armBasisOnOff)   {
                
                Leap::FloatArray array = arm.basis().toArray3x3();
                
                t_atom armBasis[12];
                
                atom_setsym(armBasis,          x->s_basis);
                atom_setlong(armBasis+1,       handID);
                atom_setlong(armBasis+2,       x->curFrameID);
                
                for (int i = 3; i < 12; i++)    {
                    
                    atom_setfloat(armBasis + i, array.m_array[i - 3]);
                }
  
                
                outlet_anything(x->outletArms, x->HAND, 12, armBasis);
            }
 
            if (x->armCenterOnOff)  {
                
                Leap::Vector center = MRleap_normalizeVec(x, frame, arm.center(), x->armCenterNormOnOff);
                
                t_atom armCenter[6];
                
                atom_setsym(armCenter,      x->s_center);
                atom_setlong(armCenter+1,   handID);
                atom_setlong(armCenter+2,   x->curFrameID);
                atom_setfloat(armCenter+3,   center.x);
                atom_setfloat(armCenter+4,   center.z);
                atom_setfloat(armCenter+5,   center.x);
                
                outlet_anything(x->outletArms, x->HAND, 6, armCenter);
            }
 
            if (x->armDirectionOnOff)   {
             
                Leap::Vector direction = arm.direction();
                
                t_atom armDirection[6];
                
                atom_setsym(armDirection,      x->s_direction);
                atom_setlong(armDirection+1,   handID);
                atom_setlong(armDirection+2,   x->curFrameID);
                atom_setfloat(armDirection+3,   direction.x);
                atom_setfloat(armDirection+4,   direction.z);
                atom_setfloat(armDirection+5,   direction.x);
                
                outlet_anything(x->outletArms, x->HAND, 6, armDirection);

            }
 
            if (x->armElbowPositionOnOff)   {
                
                Leap::Vector position = MRleap_normalizeVec(x, frame, arm.elbowPosition(), x->armElbowPositionNormOnOff);
                
                t_atom elbowPosition[6];
                
                atom_setsym(elbowPosition,      x->s_elbowPosition);
                atom_setlong(elbowPosition+1,   handID);
                atom_setlong(elbowPosition+2,   x->curFrameID);
                atom_setfloat(elbowPosition+3,   position.x);
                atom_setfloat(elbowPosition+4,   position.z);
                atom_setfloat(elbowPosition+5,   position.x);
                
                outlet_anything(x->outletArms, x->HAND, 6, elbowPosition);
            }

            if (x->armWristPositionOnOff)   {
                
                Leap::Vector position = MRleap_normalizeVec(x, frame, arm.wristPosition(), x->armWristPositionNormOnOff);
                
                t_atom wristPosition[6];
                
                atom_setsym(wristPosition,      x->s_wristPosition);
                atom_setlong(wristPosition+1,   handID);
                atom_setlong(wristPosition+2,   x->curFrameID);
                atom_setfloat(wristPosition+3,   position.x);
                atom_setfloat(wristPosition+4,   position.z);
                atom_setfloat(wristPosition+5,   position.x);
                
                outlet_anything(x->outletArms, x->HAND, 6, wristPosition);
            }

            if (x->armWidthOnOff)   {
                
                Leap::Vector wristPosition = arm.wristPosition();
                Leap::Vector elbowPosition = arm.elbowPosition();
                
                Leap::Vector displacement = elbowPosition - wristPosition;
                
                t_atom width[4];
                
                atom_setsym(width,      x->s_width);
                atom_setlong(width+1,   handID);
                atom_setlong(width+2,   x->curFrameID);
                atom_setfloat(width+3,   displacement.magnitude());
                
                outlet_anything(x->outletArms, x->HAND, 4, width);
            }
        }
    }
}
/************************************/
void MRleap_getToolData(t_MRleap *x, Leap::Frame frame)
{
    Leap::PointableList     pointables      = frame.pointables();
    const int               numPointables   = pointables.count(); //what's the max?? (have seen up to 5 so far
    
    for(int i = 0; i < numPointables; ++i)  {
        
        const Leap::Pointable point = pointables[i];
        
        if (point.isValid()) {
            
            if(point.isTool())  {
                
                MRleap_assignHandID(x, point.hand());
                
                long handID     = point.hand().id();
                long pointID    = point.id();
                
                if(x->toolMainOnOff)    {
                    
                    t_atom toolMain[5];
                    
                    atom_setsym(toolMain,       x->s_toolMain);
                    atom_setlong(toolMain+1,    pointID);
                    atom_setlong(toolMain+2,    handID);
                    atom_setlong(toolMain+3,    x->curFrameID);
                    atom_setfloat(toolMain+4,   point.timeVisible() * 1000);//sec -> ms
                    
                    outlet_anything(x->outletTools, x->HAND, 5, toolMain);
                }
                /////////////direction///////////////
                if (x->toolDirectionOnOff) {
                    
                    t_atom pointDirection[7];
                    
                    atom_setsym(pointDirection,         x->s_direction);
                    atom_setlong(pointDirection+1,      pointID);
                    atom_setlong(pointDirection+2,      handID);
                    atom_setlong(pointDirection+3,      x->curFrameID);
                    atom_setfloat(pointDirection+4,     point.direction().x);
                    atom_setfloat(pointDirection+5,     point.direction().y);
                    atom_setfloat(pointDirection+6,     point.direction().z);
                    
                    outlet_anything(x->outletTools, x->HAND, 7, pointDirection);
                }
                /////////////tip///////////////
                if (x->toolTipPositionOnOff)    {
                    
                    
                    Leap::Vector tip = MRleap_normalizeVec(x, frame, point.tipPosition(), x->toolTipNormOnOff);

                    t_atom pointTip[7];
                    
                    atom_setsym(pointTip,       x->s_tip);
                    atom_setlong(pointTip+1,    pointID);
                    atom_setlong(pointTip+2,    handID);
                    atom_setlong(pointTip+3,    x->curFrameID);
                    atom_setfloat(pointTip+4,   tip.x);
                    atom_setfloat(pointTip+5,   tip.y);
                    atom_setfloat(pointTip+6,   tip.z);
                    
                    outlet_anything(x->outletTools, x->HAND, 7, pointTip);
                }
                /////////////tip velocity///////////////
                
                if (x->toolTipVelocityOnOff)    {
                    
                    t_atom pointVel[7];
                    
                    atom_setsym(pointVel,       x->s_tipVelocity);
                    atom_setlong(pointVel+1,    pointID);
                    atom_setlong(pointVel+2,    handID);
                    atom_setlong(pointVel+3,    x->curFrameID);
                    atom_setfloat(pointVel+4,   point.tipVelocity().x);
                    atom_setfloat(pointVel+5,   point.tipVelocity().y);
                    atom_setfloat(pointVel+6,   point.tipVelocity().z);
                    
                    outlet_anything(x->outletTools, x->HAND, 7, pointVel);
                }
                ///////////////tip dimension///////////
                if (x->toolDimensionOnOff)  {
                    
                    t_atom pointDim[6];
                    
                    atom_setsym(pointDim,       x->s_tipDimension);
                    atom_setlong(pointDim+1,    pointID);
                    atom_setlong(pointDim+2,    handID);
                    atom_setlong(pointDim+3,    x->curFrameID);
                    atom_setfloat(pointDim+4,   point.width());
                    atom_setfloat(pointDim+5,   point.length());
                    
                    outlet_anything(x->outletTools, x->HAND, 6, pointDim);
                }
                ///////////////touch zone//////////////
                if (x->toolTouchZoneOnOff)  {
                    
                    t_atom pointTouch[6];
                    
                    atom_setsym(pointTouch,         x->s_touchZone);
                    atom_setlong(pointTouch+1,      pointID);
                    atom_setlong(pointTouch+2,      handID);
                    atom_setlong(pointTouch+3,      x->curFrameID);
                    atom_setfloat(pointTouch+4,     point.touchDistance());
                    atom_setlong(pointTouch+5,      point.touchZone());
                    
                    outlet_anything(x->outletTools, x->HAND, 6, pointTouch);
                }
            }
        }
    }
}
/************************************/
void MRleap_getFingerData(t_MRleap *x,  Leap::Frame frame)
{
    Leap::FingerList        fingers      = frame.fingers();
    const int               numFingers   = fingers.count(); //now always 5 with new API
    
    
    for(int i = 0; i < numFingers; ++i)  {
        
        const Leap::Finger finger = fingers[i];
        
        if (finger.isValid()) {
            
            if(finger.isFinger())  {
                

                MRleap_assignHandID(x, finger.hand());
                
                long handID     = finger.hand().id();
                long pointID    = finger.id();
                
                
                if(x->fingerMainOnOff)    {
                    
                    t_atom fingerMain[6];
                
                    atom_setsym(fingerMain,       x->s_fingerMain);
                    atom_setlong(fingerMain+1,    pointID);
                    atom_setlong(fingerMain+2,    handID);
                    atom_setlong(fingerMain+3,    x->curFrameID);
                    atom_setsym(fingerMain+4,     x->fingerNames[finger.type()]);
                    atom_setfloat(fingerMain+5,   finger.timeVisible() * 1000);//sec -> ms
                   
                    
                    outlet_anything(x->outletFingers, x->HAND, 6, fingerMain);
                }
                /////////////direction///////////////
                if (x->fingerDirectionOnOff) {
                    t_atom pointDirection[8];
                    
                    atom_setsym(pointDirection,         x->s_direction);
                    atom_setlong(pointDirection+1,      pointID);
                    atom_setlong(pointDirection+2,      handID);
                    atom_setlong(pointDirection+3,      x->curFrameID);
                    atom_setsym(pointDirection+4,       x->fingerNames[finger.type()]);
                    atom_setfloat(pointDirection+5,     finger.direction().x);
                    atom_setfloat(pointDirection+6,     finger.direction().y);
                    atom_setfloat(pointDirection+7,     finger.direction().z);
                    
                    outlet_anything(x->outletFingers, x->HAND, 8, pointDirection);
                }
                /////////////tip///////////////
                if (x->fingerTipPositionOnOff)    {
                    
                    
                    Leap::Vector tip = MRleap_normalizeVec(x, frame, finger.tipPosition(), x->fingerTipNormOnOff);
                    
                    t_atom pointTip[8];
                    
                    atom_setsym(pointTip,       x->s_tip);
                    atom_setlong(pointTip+1,    pointID);
                    atom_setlong(pointTip+2,    handID);
                    atom_setlong(pointTip+3,    x->curFrameID);
                    atom_setsym(pointTip+4,     x->fingerNames[finger.type()]);
                    atom_setfloat(pointTip+5,   tip.x);
                    atom_setfloat(pointTip+6,   tip.y);
                    atom_setfloat(pointTip+7,   tip.z);
                    
                    outlet_anything(x->outletFingers, x->HAND, 8, pointTip);
                    
                }
                /////////////tip velocity///////////////
                
                if (x->fingerTipVelocityOnOff)    {
                    
                    t_atom pointVel[8];
                    
                    atom_setsym(pointVel,       x->s_tipVelocity);
                    atom_setlong(pointVel+1,    pointID);
                    atom_setlong(pointVel+2,    handID);
                    atom_setlong(pointVel+3,    x->curFrameID);
                    atom_setsym(pointVel+4,     x->fingerNames[finger.type()]);
                    atom_setfloat(pointVel+5,   finger.tipVelocity().x);
                    atom_setfloat(pointVel+6,   finger.tipVelocity().y);
                    atom_setfloat(pointVel+7,   finger.tipVelocity().z);
                    
                    outlet_anything(x->outletFingers, x->HAND, 8, pointVel);
                }
                ///////////////tip dimension///////////
                if (x->fingerDimensionOnOff)  {
                    
                    t_atom pointDim[7];
                    
                    atom_setsym(pointDim,       x->s_tipDimension);
                    atom_setlong(pointDim+1,    pointID);
                    atom_setlong(pointDim+2,    handID);
                    atom_setlong(pointDim+3,    x->curFrameID);
                    atom_setsym(pointDim+4,     x->fingerNames[finger.type()]);
                    atom_setfloat(pointDim+5,   finger.width());
                    atom_setfloat(pointDim+6,   finger.length());
                    
                    outlet_anything(x->outletFingers, x->HAND, 7, pointDim);
                }
                if (x->fingerTouchZoneOnOff)  {
                    
                    t_atom pointTouch[7];
                    
                    atom_setsym(pointTouch,         x->s_touchZone);
                    atom_setlong(pointTouch+1,      pointID);
                    atom_setlong(pointTouch+2,      handID);
                    atom_setlong(pointTouch+3,      x->curFrameID);
                    atom_setsym(pointTouch+4,       x->fingerNames[finger.type()]);
                    atom_setfloat(pointTouch+5,     finger.touchDistance());
                    atom_setlong(pointTouch+6,      finger.touchZone());
                    
                    outlet_anything(x->outletFingers, x->HAND, 7, pointTouch);
                }
                
                if (x->fingerIsExtendedOnOff)   {
                    
                    t_atom extended[6];
                    
                    atom_setsym(extended,         x->s_extended);
                    atom_setlong(extended+1,      pointID);
                    atom_setlong(extended+2,      handID);
                    atom_setlong(extended+3,      x->curFrameID);
                    atom_setsym(extended+4,       x->fingerNames[finger.type()]);
                    atom_setlong(extended+5,      finger.isExtended());
                    
                    
                    outlet_anything(x->outletFingers, x->HAND, 6, extended);
                }
                
            }
        }
    }
}
/************************************/
void MRleap_getGestureData(t_MRleap *x, Leap::Frame frame)
{
        int gestureCount = frame.gestures().count();
        
        
        for (int i = 0; i < gestureCount; i++)  {
            
            Leap::Gesture gesture = frame.gestures()[i];
                        
            //getting gestures is expensive... really making sure to only get one if we want/need one
            if(x->leap->isGestureEnabled(Leap::Gesture::TYPE_CIRCLE) && x->gestureCircleOnOff){
     
                if(gesture.type() == Leap::Gesture::TYPE_CIRCLE && gesture.isValid())    {
                    
                    MRleap_processCircleData(x, frame, gesture);
                }
            }
    
            if(x->leap->isGestureEnabled(Leap::Gesture::TYPE_KEY_TAP) && x->gestureKeyTapOnOff){
                
                if(gesture.type() == Leap::Gesture::TYPE_KEY_TAP && gesture.isValid())    {
                    
                    MRleap_processKeyTapData(x, frame, gesture);
                }
            }
    
            if(x->leap->isGestureEnabled(Leap::Gesture::TYPE_SCREEN_TAP) &&x->gestureScreenTapOnOff){
                
                if(gesture.type() == Leap::Gesture::TYPE_SCREEN_TAP && gesture.isValid())    {
                    
                    MRleap_processScreenTapData(x, frame, gesture);
                }
            }
    
            if(x->leap->isGestureEnabled(Leap::Gesture::TYPE_SWIPE) && x->gestureSwipeOnOff){
                
                if(gesture.type() == Leap::Gesture::TYPE_SWIPE && gesture.isValid())    {
                    
                    MRleap_processSwipeData(x, frame, gesture);
                }
            }
        }
}
/************************************/
void MRleap_processCircleData(t_MRleap *x, Leap::Frame frame, Leap::Gesture gesture)
{
    Leap::CircleGesture circle = Leap::CircleGesture(gesture);
    Leap::Pointable circlePointable = circle.pointable();
    
   if (circlePointable.isFinger())  {

       MRleap_assignHandID(x, circlePointable.hand());
    
    if (x->gestureCircleMainOnOff)  {
        
        t_atom circleMain [6];
        
        atom_setsym(circleMain,     x->s_circleMain);
        atom_setlong(circleMain+1,  circle.id());
        atom_setlong(circleMain+2,  circlePointable.id());
        atom_setlong(circleMain+3,  circlePointable.hand().id());
        atom_setlong(circleMain+4,  x->curFrameID);
        atom_setlong(circleMain+5,  circle.duration() * 0.001); //microsec -> ms
        
        outlet_anything(x->outletGestures, x->HAND, 6, circleMain);
    }
    
    
    if (x->gestureCircleCenterOnOff)    {
        
        Leap::Vector center = MRleap_normalizeVec(x, frame, circle.center(), x->gestureCircleCenterNormOnOff);
        
        t_atom circleCenter[8];
        
        atom_setsym(circleCenter,       x->s_circleCenter);
        atom_setlong(circleCenter+1,    circle.id());
        atom_setlong(circleCenter+2,    circlePointable.id());
        atom_setlong(circleCenter+3,    circlePointable.hand().id());
        atom_setlong(circleCenter+4,    x->curFrameID);
        atom_setfloat(circleCenter+5,   center.x);
        atom_setfloat(circleCenter+6,   center.y);
        atom_setfloat(circleCenter+7,   center.z);
        
        outlet_anything(x->outletGestures, x->HAND, 8, circleCenter);
    }
    
    
    if (x->gestureCircleDataOnOff)  {
        
        
        float turns = circle.progress();
        
        float diameter = 2 * circle.radius();
        
        int clockwiseness;
        
        if (circlePointable.direction().angleTo(circle.normal()) <= Leap::PI/2) {
            clockwiseness = 1;
        }
        else
        {
            clockwiseness = -1;
        }
        
        t_atom circleData[9];
        
        atom_setsym(circleData,         x->s_circleData);
        atom_setlong(circleData+1,      circle.id());
        atom_setlong(circleData+2,      circlePointable.id());
        atom_setlong(circleData+3,      circlePointable.hand().id());
        atom_setlong(circleData+4,      x->curFrameID);
        atom_setlong (circleData+5,     circle.state());
        atom_setfloat(circleData+6,     turns);
        atom_setfloat(circleData+7,     diameter);
        atom_setfloat(circleData+8,     clockwiseness);
        
        outlet_anything(x->outletGestures, x->HAND, 9, circleData);
    }
   }
}
/************************************/
/************************************/
void MRleap_circleGestureReset(t_MRleap *x)
{
    x->circleMinRadius              = x->circleMinRadiusDef;
    x->circleMinArc                 = x->circleMinArcDef;
    
    x->leap->config().setFloat("Gesture.Circle.MinRadius",              x->circleMinRadius);
    x->leap->config().setFloat("Gesture.Circle.MinArc",                 x->circleMinArc);
    x->leap->config().save();
    
    post("CircleGesture has been reset to the default values of:");
    post("radius %f and arc %f", x->circleMinRadius, x->circleMinArc);
}
/************************************/
void MRleap_keyTapGestureReset(t_MRleap *x)
{
    x->keyTapMinDownVelocity        = x->keyTapMinDownVelocityDef;
    x->keyTapHistorySeconds         = x->keyTapHistorySecondsDef;
    x->keyTapMinDistance            = x->keyTapMinDistanceDef;
    
    x->leap->config().setFloat("Gesture.KeyTap.MinDownVelocity",        x->keyTapMinDownVelocity);
    x->leap->config().setFloat("Gesture.KeyTap.HistorySeconds",         x->keyTapHistorySeconds);
    x->leap->config().setFloat("Gesture.KeyTap.MinDistance",            x->keyTapMinDistance);
    x->leap->config().save();
    
    post("KeyTapGesture has been reset to the default values of:");
    post("velocity %f seconds %f and distance %f", x->keyTapMinDownVelocity, x->keyTapHistorySeconds, x->keyTapMinDistance);
}
/************************************/
void MRleap_screenTapGestureReset(t_MRleap *x)
{
    x->screenTapMinForwardVelocity  = x->screenTapMinForwardVelocityDef;
    x->screenTapHistorySeconds      = x->screenTapHistorySecondsDef;
    x->screenTapMinDistance         = x->screenTapMinDistanceDef;
    
    x->leap->config().setFloat("Gesture.ScreenTap.MinForwardVelocity",  x->screenTapMinForwardVelocity);
    x->leap->config().setFloat("Gesture.ScreenTap.HistorySeconds",      x->screenTapHistorySeconds);
    x->leap->config().setFloat("Gesture.ScreenTap.MinDistance",         x->screenTapMinDistance);
    x->leap->config().save();
    
    post("ScreenTapGesture has been reset to the default values of:");
    post("velocity %f seconds %f and distance %f", x->screenTapMinForwardVelocity, x->screenTapHistorySeconds, x->screenTapMinDistance);
}
/************************************/
void MRleap_swipeGestureReset(t_MRleap *x)
{
    x->swipeMinLength               = x->swipeMinLengthDef;
    x->swipeMinVelocity             = x->swipeMinVelocityDef;
    
    x->leap->config().setFloat("Gesture.Swipe.MinLength",               x->swipeMinLength);
    x->leap->config().setFloat("Gesture.Swipe.MinVelocity",             x->swipeMinVelocity);
    x->leap->config().save();
    
    post("SwipeGesture has been reset to the default values of:");
    post("length %f and velocity %f", x->swipeMinLength, x->swipeMinVelocity);
}
/************************************/
void MRleap_processKeyTapData(t_MRleap *x, Leap::Frame frame, Leap::Gesture gesture)
{
    Leap::KeyTapGesture tap = Leap::KeyTapGesture(gesture);
    Leap::Pointable tapPointable = tap.pointable();
    
    MRleap_assignHandID(x, tapPointable.hand());
    
    if (x->gestureKeyTapMainOnOff)  {
        
        t_atom tapMain [6];
        
        atom_setsym(tapMain,       x->s_keyTapMain);
        atom_setlong(tapMain+1,    tap.id());
        atom_setlong(tapMain+2,    tapPointable.id());
        atom_setlong(tapMain+3,    tapPointable.hand().id());
        atom_setlong(tapMain+4,    x->curFrameID);
        atom_setlong(tapMain+5,    tap.duration() * 0.001); //microsec -> ms
        
        outlet_anything(x->outletGestures, x->HAND, 6, tapMain);
    }
    
    
    if (x->gestureKeyTapPositionOnOff)    {
        
        Leap::Vector pos = MRleap_normalizeVec(x, frame, tap.position(), x->gestureKeyTapPositionNormOnOff);

        t_atom tapPos[9];
        
        atom_setsym(tapPos,      x->s_keyTapPosition);
        atom_setlong(tapPos+1,   tap.id());
        atom_setlong(tapPos+2,   tapPointable.id());
        atom_setlong(tapPos+3,   tapPointable.hand().id());
        atom_setlong(tapPos+4,   x->curFrameID);
        atom_setlong (tapPos+5,  tap.state());
        atom_setfloat(tapPos+6,  pos.x);
        atom_setfloat(tapPos+7,  pos.y);
        atom_setfloat(tapPos+8,  pos.z);
        
        outlet_anything(x->outletGestures, x->HAND, 9, tapPos);
    }
    
    
    if (x->gestureKeyTapDirectionOnOff)  {
        
        
        t_atom keyTapDirection[8];
        
        atom_setsym(keyTapDirection,        x->s_keyTapDirection);
        atom_setlong(keyTapDirection+1,     tap.id());
        atom_setlong(keyTapDirection+2,     tapPointable.id());
        atom_setlong(keyTapDirection+3,     tapPointable.hand().id());
        atom_setlong(keyTapDirection+4,     x->curFrameID);
        atom_setlong (keyTapDirection+5,    tap.direction().x);
        atom_setfloat(keyTapDirection+6,    tap.direction().y);
        atom_setfloat(keyTapDirection+7,    tap.direction().z);
        
        outlet_anything(x->outletGestures, x->HAND, 8, keyTapDirection);
    }
}
/************************************/
void MRleap_processScreenTapData(t_MRleap *x, Leap::Frame frame, Leap::Gesture gesture)
{
    Leap::ScreenTapGesture tap = Leap::ScreenTapGesture(gesture);
    Leap::Pointable tapPointable = tap.pointable();
    
    MRleap_assignHandID(x, tapPointable.hand());
    
    if (x->gestureScreenTapMainOnOff)  {
        
        t_atom tapMain [6];
        
        atom_setsym(tapMain,       x->s_screenTapMain);
        atom_setlong(tapMain+1,    tap.id());
        atom_setlong(tapMain+2,    tapPointable.id());
        atom_setlong(tapMain+3,    tapPointable.hand().id());
        atom_setlong(tapMain+4,    x->curFrameID);
        atom_setlong(tapMain+5,    tap.duration() * 0.001); //microsec -> ms
        
        outlet_anything(x->outletGestures, x->HAND, 6, tapMain);
    }
    
    
    if (x->gestureScreenTapPositionOnOff)    {
        
        Leap::Vector pos = MRleap_normalizeVec(x, frame, tap.position(), x->gestureScreenTapPositionNormOnOff);
        
        t_atom tapPos[9];
        
        atom_setsym(tapPos,      x->s_screenTapPosition);
        atom_setlong(tapPos+1,   tap.id());
        atom_setlong(tapPos+2,   tapPointable.id());
        atom_setlong(tapPos+3,   tapPointable.hand().id());
        atom_setlong(tapPos+4,   x->curFrameID);
        atom_setlong (tapPos+5,  tap.state());
        atom_setfloat(tapPos+6,  pos.x);
        atom_setfloat(tapPos+7,  pos.y);
        atom_setfloat(tapPos+8,  pos.z);
        
        outlet_anything(x->outletGestures, x->HAND, 9, tapPos);
    }
    
    
    if (x->gestureScreenTapDirectionOnOff)  {
        
        
        t_atom screenTapDirection[8];
        
        atom_setsym(screenTapDirection,        x->s_screenTapDirection);
        atom_setlong(screenTapDirection+1,     tap.id());
        atom_setlong(screenTapDirection+2,     tapPointable.id());
        atom_setlong(screenTapDirection+3,     tapPointable.hand().id());
        atom_setlong(screenTapDirection+4,     x->curFrameID);
        atom_setlong (screenTapDirection+5,    tap.direction().x);
        atom_setfloat(screenTapDirection+6,    tap.direction().y);
        atom_setfloat(screenTapDirection+7,    tap.direction().z);
        
        outlet_anything(x->outletGestures, x->HAND, 8, screenTapDirection);
    }
}
/************************************/
void MRleap_processSwipeData(t_MRleap *x, Leap::Frame frame, Leap::Gesture gesture)
{
    Leap::SwipeGesture  swipe = Leap::SwipeGesture(gesture);
    Leap::Pointable     swipePointable = swipe.pointable();
    
    MRleap_assignHandID(x, swipePointable.hand());
    
    if (x->gestureSwipeMainOnOff)  {
        
        t_atom swipeMain [6];
        
        atom_setsym(swipeMain,     x->s_swipeMain);
        atom_setlong(swipeMain+1,  swipe.id());
        atom_setlong(swipeMain+2,  swipePointable.id());
        atom_setlong(swipeMain+3,  swipePointable.hand().id());
        atom_setlong(swipeMain+4,  x->curFrameID);
        atom_setlong(swipeMain+5,  swipe.duration() * 0.001); //microsec -> ms
        
        outlet_anything(x->outletGestures, x->HAND, 6, swipeMain);
    }
    
    
    if (x->gestureSwipePositionOnOff)    {
        
        Leap::Vector start      = MRleap_normalizeVec(x, frame, swipe.startPosition(), x->gestureSwipePositionNormOnOff);
        Leap::Vector current    = MRleap_normalizeVec(x, frame, swipe.position(), x->gestureSwipePositionNormOnOff);
        
        t_atom swipePos[12];
       
        atom_setsym(swipePos,      x->s_swipePosition);
        atom_setlong(swipePos+1,   swipe.id());
        atom_setlong(swipePos+2,   swipePointable.id());
        atom_setlong(swipePos+3,   swipePointable.hand().id());
        atom_setlong(swipePos+4,   x->curFrameID);
        atom_setlong (swipePos+5,  swipe.state());
        atom_setfloat(swipePos+6,  start.x);
        atom_setfloat(swipePos+7,  start.y);
        atom_setfloat(swipePos+8,  start.z);
        atom_setfloat(swipePos+9,  current.x);
        atom_setfloat(swipePos+10, current.y);
        atom_setfloat(swipePos+11, current.z);
        
        outlet_anything(x->outletGestures, x->HAND, 12, swipePos);
    }
    
    if (x->gestureSwipeDirectionOnOff)  {
        
        t_atom swipeData[9];
        
        atom_setsym(swipeData,      x->s_swipeDirection);
        atom_setlong(swipeData+1,   swipe.id());
        atom_setlong(swipeData+2,   swipePointable.id());
        atom_setlong(swipeData+3,   swipePointable.hand().id());
        atom_setlong(swipeData+4,   x->curFrameID);
        atom_setfloat(swipeData+5,  swipe.speed());
        atom_setfloat(swipeData+6,  swipe.direction().x);
        atom_setfloat(swipeData+7,  swipe.direction().y);
        atom_setfloat(swipeData+8,  swipe.direction().z);
        
        outlet_anything(x->outletGestures, x->HAND, 9, swipeData);
    }
}
/************************************/
int MRleap_biggestRotAxis(t_MRleap *x, Leap::Vector vec)
{
    float   largest = -1;
    int     which = -1;
    for (int i = 0; i < 3; i++) {
        
        if (fabs(vec[i]) > largest)   {
            
            largest = fabs(vec[i]);
            which = i;
        }
    }
    
    return which;
}
/************************************/
float MRleap_RadDeg(t_MRleap *x)
{
    
    if (x->allDegOnOff) {
        
        return Leap::RAD_TO_DEG;
    }
    
    else if (x->allDegOnOff == 0)   {
        
        return 1;
    }
    
    return 1;
}
/************************************/
float MRleap_DegRad(t_MRleap *x)
{
    
    if (x->allDegOnOff) {
        
        return 1;
    }
    
    else if (x->allDegOnOff == 0)   {
        
        return Leap::DEG_TO_RAD;
    }
    
    return 1;
}
/************************************/
Leap::Vector MRleap_normalizeVec(t_MRleap *x, Leap::Frame frame, Leap::Vector vec, long is)
{
    Leap::Vector out = vec;
    
    if (is) {
        
        out       = frame.interactionBox().normalizePoint(vec);
        
        return out;
    }
    
    else return out;
    
    return out = Leap::Vector::zero(); //should never been called
}
/************************************/
void MRleap_gestureResetAll(t_MRleap *x)
{
    x->circleMinRadius              = x->circleMinRadiusDef;
    x->circleMinArc                 = x->circleMinArcDef;
    x->swipeMinLength               = x->swipeMinLengthDef;
    x->swipeMinVelocity             = x->swipeMinVelocityDef;
    x->keyTapMinDownVelocity        = x->keyTapMinDownVelocityDef;
    x->keyTapHistorySeconds         = x->keyTapHistorySecondsDef;
    x->keyTapMinDistance            = x->keyTapMinDistanceDef;
    x->screenTapMinForwardVelocity  = x->screenTapMinForwardVelocityDef;
    x->screenTapHistorySeconds      = x->screenTapHistorySecondsDef;
    x->screenTapMinDistance         = x->screenTapMinDistanceDef;
    
    x->leap->config().setFloat("Gesture.Circle.MinRadius",              x->circleMinRadius);
    x->leap->config().setFloat("Gesture.Circle.MinArc",                 x->circleMinArc);
    x->leap->config().setFloat("Gesture.Swipe.MinLength",               x->swipeMinLength);
    x->leap->config().setFloat("Gesture.Swipe.MinVelocity",             x->swipeMinVelocity);
    x->leap->config().setFloat("Gesture.KeyTap.MinDownVelocity",        x->keyTapMinDownVelocity);
    x->leap->config().setFloat("Gesture.KeyTap.HistorySeconds",         x->keyTapHistorySeconds);
    x->leap->config().setFloat("Gesture.KeyTap.MinDistance",            x->keyTapMinDistance);
    x->leap->config().setFloat("Gesture.ScreenTap.MinForwardVelocity",  x->screenTapMinForwardVelocity);
    x->leap->config().setFloat("Gesture.ScreenTap.HistorySeconds",      x->screenTapHistorySeconds);
    x->leap->config().setFloat("Gesture.ScreenTap.MinDistance",         x->screenTapMinDistance);
    x->leap->config().save();
    
    post("All gesture attributes have been reset to their original values...");
}
/************************************/
void MRleap_gestureResetGeneric(t_MRleap *x, t_symbol *gesture)
{
    if (gesture == gensym("circleMinRadius"))   {
        x->circleMinRadius              = x->circleMinRadiusDef;
        x->leap->config().setFloat("Gesture.Circle.MinRadius",              x->circleMinRadius);
        
        post("%s has been reset to the default value of: %f", gesture->s_name, x->circleMinRadius);
    }
    else if (gesture == gensym("circleMinArc"))  {
        x->circleMinArc                 = x->circleMinArcDef;
        x->leap->config().setFloat("Gesture.Circle.MinArc",                 x->circleMinArc);
        
        post("%s has been reset to the default value of: %f", gesture->s_name, x->circleMinArc);
    }
    else if (gesture == gensym("swipeMinLength"))  {
        x->swipeMinLength               = x->swipeMinLengthDef;
        x->leap->config().setFloat("Gesture.Swipe.MinLength",               x->swipeMinLength);
        
        post("%s has been reset to the default value of: %f", gesture->s_name, x->swipeMinLength);
    }
    else if (gesture == gensym("swipeMinVelocity"))  {
        x->swipeMinVelocity             = x->swipeMinVelocityDef;
        x->leap->config().setFloat("Gesture.Swipe.MinVelocity",             x->swipeMinVelocity);
        
        post("%s has been reset to the default value of: %f", gesture->s_name, x->swipeMinVelocity);
    }
    else if (gesture == gensym("keyTapMinDownVelocity"))  {
        x->keyTapMinDownVelocity        = x->keyTapMinDownVelocityDef;
        x->leap->config().setFloat("Gesture.KeyTap.MinDownVelocity",        x->keyTapMinDownVelocity);
        
        post("%s has been reset to the default value of: %f", gesture->s_name, x->keyTapMinDownVelocity);
    }
    else if (gesture == gensym("keyTapHistorySeconds"))  {
        x->keyTapHistorySeconds         = x->keyTapHistorySecondsDef;
        x->leap->config().setFloat("Gesture.KeyTap.HistorySeconds",         x->keyTapHistorySeconds);
        
        post("%s has been reset to the default value of: %f", gesture->s_name, x->keyTapHistorySeconds);
    }
    else if (gesture == gensym("keyTapMinDistance"))  {
        x->keyTapMinDistance            = x->keyTapMinDistanceDef;
        x->leap->config().setFloat("Gesture.KeyTap.MinDistance",            x->keyTapMinDistance);
        
        post("%s has been reset to the default value of: %f", gesture->s_name, x->keyTapMinDistance);
    }
    else if (gesture == gensym("screenTapMinForwardVelocity"))  {
        x->screenTapMinForwardVelocity  = x->screenTapMinForwardVelocityDef;
        x->leap->config().setFloat("Gesture.ScreenTap.MinForwardVelocity",  x->screenTapMinForwardVelocity);
        
        post("%s has been reset to the default value of: %f", gesture->s_name, x->screenTapMinForwardVelocity);
    }
    else if (gesture == gensym("screenTapHistorySeconds"))  {
        x->screenTapHistorySeconds      = x->screenTapHistorySecondsDef;
        x->leap->config().setFloat("Gesture.ScreenTap.HistorySeconds",      x->screenTapHistorySeconds);
        
        post("%s has been reset to the default value of: %f", gesture->s_name, x->screenTapHistorySeconds);
    }
    else if (gesture == gensym("screenTapMinDistance"))  {
        x->screenTapMinDistance         = x->screenTapMinDistanceDef;
        x->leap->config().setFloat("Gesture.ScreenTap.MinDistance",         x->screenTapMinDistance);
        
        post("%s has been reset to the default value of: %f", gesture->s_name, x->screenTapMinDistance);
    }
    else {
        
        object_error((t_object *) x, "ERROR: The symbol %s\n is not supported", gesture->s_name);
    }
    
     x->leap->config().save();
}
/************************************/
t_max_err MRleap_gestureCircleMinArc_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->circleMinArc = atom_getfloat(argv) * MRleap_RadDeg(x);
        
        x->leap->config().setFloat("Gesture.Circle.MinArc", x->circleMinArc);
        x->leap->config().save();
        
        return MAX_ERR_NONE;
    }
    
    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureCircleMinRadius_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->circleMinRadius = atom_getfloat(argv);
        
        x->leap->config().setFloat("Gesture.Circle.MinRadius", x->circleMinRadius);
        x->leap->config().save();
        
        return MAX_ERR_NONE;
    }

    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureSwipeMinLength_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->swipeMinLength = atom_getfloat(argv);
        
        x->leap->config().setFloat("Gesture.Swipe.MinLength", x->swipeMinLength);
        x->leap->config().save();
        
        return MAX_ERR_NONE;
    }

    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureSwipeMinVelocity_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->swipeMinVelocity = atom_getfloat(argv);
        
        x->leap->config().setFloat("Gesture.Swipe.MinVelocity", x->swipeMinVelocity);
        x->leap->config().save();
        
        return MAX_ERR_NONE;
    }

    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureKeyTapMinDownVelocity_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->keyTapMinDownVelocity = atom_getfloat(argv);
        
        x->leap->config().setFloat("Gesture.KeyTap.MinDownVelocity", x->keyTapMinDownVelocity);
        x->leap->config().save();
        
        return MAX_ERR_NONE;
    }

    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureKeyTapMinDistance_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->keyTapMinDistance = atom_getfloat(argv);
        
        x->leap->config().setFloat("Gesture.KeyTap.MinDistance", x->keyTapMinDistance);
        x->leap->config().save();
        
        return MAX_ERR_NONE;
    }

    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureKeyTapHistorySeconds_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->keyTapHistorySeconds = atom_getfloat(argv);
        
        x->leap->config().setFloat("Gesture.KeyTap.HistorySeconds", x->keyTapHistorySeconds);
        x->leap->config().save();
        
        return MAX_ERR_NONE;
    }

    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureScreenTapMinForwardVelocity_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->screenTapMinForwardVelocity = atom_getfloat(argv);
        
        x->leap->config().setFloat("Gesture.ScreenTap.MinForwardVelocity", x->screenTapMinForwardVelocity);
        x->leap->config().save();
        
        return MAX_ERR_NONE;
    }

    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureScreenTapHistorySeconds_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->screenTapHistorySeconds = atom_getfloat(argv);
        
        x->leap->config().setFloat("Gesture.ScreenTap.HistorySeconds", x->screenTapHistorySeconds);
        x->leap->config().save();
        
        return MAX_ERR_NONE;
    }

    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureScreenTapMinDistance_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->screenTapMinDistance = atom_getfloat(argv);
        
        x->leap->config().setFloat("Gesture.ScreenTap.MinDistance", x->screenTapMinDistance);
        x->leap->config().save();
        
        return MAX_ERR_NONE;
    }

    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureCircleOnOff_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->gestureCircleOnOff = atom_getlong(argv);
        
        x->leap->enableGesture(Leap::Gesture::TYPE_CIRCLE, x->gestureCircleOnOff);
        
        return MAX_ERR_NONE;
    }
    
    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureKeyTapOnOff_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->gestureKeyTapOnOff = atom_getlong(argv);
        
        x->leap->enableGesture(Leap::Gesture::TYPE_KEY_TAP, x->gestureKeyTapOnOff);
        
        return MAX_ERR_NONE;
    }
    
    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureScreenTapOnOff_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->gestureScreenTapOnOff = atom_getlong(argv);
        
        x->leap->enableGesture(Leap::Gesture::TYPE_SCREEN_TAP, x->gestureScreenTapOnOff);
        
        return MAX_ERR_NONE;
    }
    
    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureSwipeOnOff_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->gestureSwipeOnOff = atom_getlong(argv);
        
        x->leap->enableGesture(Leap::Gesture::TYPE_SWIPE, x->gestureSwipeOnOff);
        
        return MAX_ERR_NONE;
    }
    
    return MAX_ERR_GENERIC;
}
/************************************/
void MRleap_assignHandID(t_MRleap *x, Leap::Hand hand)
{
    
    if (hand.isLeft())  {
        
        x->HAND = x->LEFT;
    }
    else if (hand.isRight())    {
        
        x->HAND = x->RIGHT;
    }
    else {
        
        x->HAND = x->OTHER;
    }
}
/************************************/