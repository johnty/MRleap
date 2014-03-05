/**
 t_max_err myobjet_setattr_myattr(t_myobject *x, void *attr, long ac, t_atom *av)
 {
 if (ac && av) {
 if (atom_gettype(av) == A_LONG || atom_gettype(av) == A_FLOAT) {
 x->myattr = *av;
 else
 atom_setsym(&x->myattr, gensym(""));
 } else
 atom_setsym(&x->myattr, gensym(""));
 
 return MAX_ERR_NONE;
 }
 */


#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object
#include "Leap.h"
#include <math.h>
#include "LeapMath.h"

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2


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


 const float circleMinRadiusDef               = 5.;
 const float circleMinArcDef                  = 1.5 * Leap::PI;
 const float swipeMinLengthDef                = 150;
 const float swipeMinVelocityDef              = 1000;
 const float keyTapMinDownVelocityDef         = 50;
 const float keyTapHistorySecondsDef          = 0.1;
 const float keyTapMinDistanceDef             = 3;
 const float screenTapMinForwardVelocityDef   = 50;
 const float screenTapHistorySecondsDef       = 0.1;
 const float screenTapMinDistanceDef          = 5;


/************************************/
typedef struct _MRleap
{
	t_object            ob;
    
    
    long             prevFrameID;
    long             curFrameID;
    
    long                allDegOnOff;
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
    
    long                leftmostOnOff;
    long                leftmostID;
    long                prevLeftmostID;
    long                rightmostOnOff;
    long                rightmostID;
    long                prevRightmostID;
    
//    Leap::Matrix        mtxTotalMotionRotation;
  //  Leap::Vector        vecTotalMotionTranslation;
    //float               fTotalMotionScale;
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
    
    float               handRotationProb;
    float               handScaleProb;
    float               handTranslationProb;
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
    
    ///////////////
    /////outlets
    void                *outletStart;
	void                *outletFrame;
    void                *outletHands;
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

void MRleap_gestureResetAll(t_MRleap *x);


t_max_err MRleap_gestureCircleMinArc_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureCircleMinArc_get(t_MRleap *x, t_object *attr, long *argc, t_atom **argv);

t_max_err MRleap_gestureCircleOnOff_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureKeyTapOnOff_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureScreenTapOnOff_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);
t_max_err MRleap_gestureSwipeOnOff_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv);

int MRleap_biggestRotAxis(t_MRleap *x, Leap::Vector vec);

void MRleap_bang(t_MRleap *x);
/************************************/
static t_class *MRleap_class;


/************************************/
int T_EXPORT main(void)
{
	t_class *c;
	
	c = class_new("MRleap", (method)MRleap_new, (method)MRleap_free, sizeof(t_MRleap), NULL, A_GIMME,   0);
    class_addmethod(c, (method)MRleap_bang,                     "bang",                                 0);
    class_addmethod(c, (method)MRleap_assist,                   "assist",                    A_CANT,    0);
    class_addmethod(c, (method)MRleap_gestureResetAll,          "resetGestureAll",           A_NOTHING, 0);
    
    //////////////attribute
    /***************************Global***********************************/
    CLASS_ATTR_LONG(c, "frameHist", 0, t_MRleap, frameHist);
	CLASS_ATTR_LABEL(c, "frameHist", 0, "FrameHist");
    CLASS_ATTR_FILTER_CLIP(c, "frameHist", 1, 59);
	CLASS_ATTR_SAVE(c,"frameHist", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "allDegOnOff", 0, t_MRleap, allDegOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "allDegOnOff", 0, "onoff", "Degrees");
	CLASS_ATTR_SAVE(c,"allDegOnOff", 0);
    /************************** FRAME ********************************/
    CLASS_ATTR_LONG(c, "frameMainOnOff", 0, t_MRleap, frameMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameMainOnOff", 0, "onoff", "FrameMain");
	CLASS_ATTR_SAVE(c,"frameMainOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "frameRotOnOff", 0, t_MRleap, frameRotOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameRotOnOff", 0, "onoff", "FrameRot");
	CLASS_ATTR_SAVE(c,"frameRotOnOff", 0);
   
	CLASS_ATTR_LONG(c, "frameRotRawOnOff", 0, t_MRleap, frameRotRawOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameRotRawOnOff", 0, "onoff", "FrameRotRaw");
	CLASS_ATTR_SAVE(c,"frameRotRawOnOff", 0);
    
    CLASS_ATTR_FLOAT(c, "frameRotProb", 0, t_MRleap, frameRotProb);
	CLASS_ATTR_LABEL(c, "frameRotProb", 0, "FrameRotProb");
    CLASS_ATTR_FILTER_CLIP(c, "frameRotProb", 0, 1);
	CLASS_ATTR_SAVE(c,"frameRotProb", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "frameScaleOnOff", 0, t_MRleap, frameScaleOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameScaleOnOff", 0, "onoff", "FrameScale");
	CLASS_ATTR_SAVE(c,"frameScaleOnOff", 0);

	CLASS_ATTR_LONG(c, "frameScaleRawOnOff", 0, t_MRleap, frameScaleRawOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameScaleRawOnOff", 0, "onoff", "FrameScaleRaw");
	CLASS_ATTR_SAVE(c,"frameScaleRawOnOff", 0);

    CLASS_ATTR_FLOAT(c, "frameScaleProb", 0, t_MRleap, frameScaleProb);
	CLASS_ATTR_LABEL(c, "frameScaleProb", 0, "FrameScaleProb");
    CLASS_ATTR_FILTER_CLIP(c, "frameScaleProb", 0, 1);
	CLASS_ATTR_SAVE(c,"frameScaleProb", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "frameTranslationOnOff", 0, t_MRleap, frameTranslationOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameTranslationOnOff", 0, "onoff", "FrameTrans");
	CLASS_ATTR_SAVE(c,"frameTranslationOnOff", 0);

    CLASS_ATTR_LONG(c, "frameTranslationRawOnOff", 0, t_MRleap, frameTranslationRawOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameTranslationRawOnOff", 0, "onoff", "FrameTransRaw");
	CLASS_ATTR_SAVE(c,"frameTranslationRawOnOff", 0);
     
    CLASS_ATTR_LONG(c, "frameTranslationNormOnOff", 0, t_MRleap, frameTranslationNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "frameTranslationNormOnOff", 0, "onoff", "FrameTransNorm");
	CLASS_ATTR_SAVE(c,"frameTranslationNormOnOff", 0);
    
    CLASS_ATTR_FLOAT(c, "frameTranslationProb", 0, t_MRleap, frameTranslationProb);
	CLASS_ATTR_LABEL(c, "frameTranslationProb", 0, "FrameTransProb");
    CLASS_ATTR_FILTER_CLIP(c, "frameTranslationProb", 0, 1);
	CLASS_ATTR_SAVE(c,"frameTranslationProb", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "frameRotationMatrixOnOff", 0, t_MRleap, frameRotationMatrix);
	CLASS_ATTR_STYLE_LABEL(c, "frameRotationMatrixOnOff", 0, "onoff", "FrameRotMatrix");
	CLASS_ATTR_SAVE(c,"frameRotationMatrixOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "rightmostOnOff", 0, t_MRleap, rightmostOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "rightmostOnOff", 0, "onoff", "RightMost");
	CLASS_ATTR_SAVE(c,"rightmostOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "leftmostOnOff", 0, t_MRleap, leftmostOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "leftmostOnOff", 0, "onoff", "LeftMost");
	CLASS_ATTR_SAVE(c,"leftmostOnOff", 0);
    /*********************************************************************/
    /************************** HAND *************************************/
    CLASS_ATTR_LONG(c, "handMainOnOff", 0, t_MRleap, handMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handMainOnOff", 0, "onoff", "HandMain");
	CLASS_ATTR_SAVE(c,"handMainOnOff", 0);
    //////////////////////////////////////////////////////////////////////
	CLASS_ATTR_LONG(c, "handSphereOnOff", 0, t_MRleap, handSphereOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handSphereOnOff", 0, "onoff", "HandSphere");
	CLASS_ATTR_SAVE(c,"handSphereOnOff", 0);

    CLASS_ATTR_LONG(c, "handSphereNormOnOff", 0, t_MRleap, handSphereNorm);
	CLASS_ATTR_STYLE_LABEL(c, "handSphereNormOnOff", 0, "onoff", "HandSphereNorm");
	CLASS_ATTR_SAVE(c,"handSphereNormOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "handPositionOnOff", 0, t_MRleap, handPositionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handPositionOnOff", 0, "onoff", "HandPosition");
	CLASS_ATTR_SAVE(c,"handPositionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "handPositionNormOnOff", 0, t_MRleap, handPositionNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handPositionNormOnOff", 0, "onoff", "HandPosNorm");
	CLASS_ATTR_SAVE(c,"handPositionNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "handPositionVelocityOnOff", 0, t_MRleap, handPositionVelocityOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handPositionVelocityOnOff", 0, "onoff", "HandVelocity");
	CLASS_ATTR_SAVE(c,"handPositionVelocityOnOff", 0);
    
    CLASS_ATTR_LONG(c, "handPositionStabilizationOnOff", 0, t_MRleap, handPositionStabilizationOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handPositionStabilizationOnOff", 0, "onoff", "HandPosStable");
	CLASS_ATTR_SAVE(c,"handPositionStabilizationOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "handRotationOnOff", 0, t_MRleap, handRotationOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handRotationOnOff", 0, "onoff", "HandRot");
	CLASS_ATTR_SAVE(c,"handRotationOnOff", 0);

	CLASS_ATTR_LONG(c, "handRotationRawOnOff", 0, t_MRleap, handRotationRawOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handRotationRawOnOff", 0, "onoff", "HandRotRaw");
	CLASS_ATTR_SAVE(c,"handRotationRawOnOff", 0);
    
    CLASS_ATTR_FLOAT(c, "handRotationProb", 0, t_MRleap, handRotationProb);
	CLASS_ATTR_LABEL(c, "handRotationProb", 0, "HandRotProb");
    CLASS_ATTR_FILTER_CLIP(c, "handRotationProb", 0, 1);
	CLASS_ATTR_SAVE(c,"handRotationProb", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "handScaleOnOff", 0, t_MRleap, handScaleOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handScaleOnOff", 0, "onoff", "HandScale");
	CLASS_ATTR_SAVE(c,"handScaleOnOff", 0);

	CLASS_ATTR_LONG(c, "handScaleRawOnOff", 0, t_MRleap, handScaleRawOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handScaleRawOnOff", 0, "onoff", "HandScaleRaw");
	CLASS_ATTR_SAVE(c,"handScaleRawOnOff", 0);

    CLASS_ATTR_FLOAT(c, "handScaleProb", 0, t_MRleap, handScaleProb);
	CLASS_ATTR_LABEL(c, "handScaleProb", 0, "HandScaleProb");
    CLASS_ATTR_FILTER_CLIP(c, "handScaleProb", 0, 1);
	CLASS_ATTR_SAVE(c,"handScaleProb", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "handTranslationOnOff", 0, t_MRleap, handTranslationOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handTranslationOnOff", 0, "onoff", "HandTrans");
	CLASS_ATTR_SAVE(c,"handTranslationOnOff", 0);

    CLASS_ATTR_LONG(c, "handTranslationRawOnOff", 0, t_MRleap, handTranslationRawOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handTranslationRawOnOff", 0, "onoff", "HandTransRaw");
	CLASS_ATTR_SAVE(c,"handTranslationRawOnOff", 0);

    CLASS_ATTR_LONG(c, "handTranslationNormOnOff", 0, t_MRleap, handTranslationNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handTranslationNormOnOff", 0, "onoff", "HandTransNorm");
	CLASS_ATTR_SAVE(c,"handTranslationNormOnOff", 0);

    CLASS_ATTR_FLOAT(c, "handTranslationProb", 0, t_MRleap, handTranslationProb);
	CLASS_ATTR_LABEL(c, "handTranslationProb", 0, "HandTransProb");
    CLASS_ATTR_FILTER_CLIP(c, "handTranslation  Prob", 0, 1);
	CLASS_ATTR_SAVE(c,"handTranslationProb", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "handRotationMatrixOnOff", 0, t_MRleap, handRotationMatrix);
	CLASS_ATTR_STYLE_LABEL(c, "handRotationMatrixOnOff", 0, "onoff", "HandRotMatrix");
	CLASS_ATTR_SAVE(c,"handRotationMatrixOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "handHorizontalPlaneOnOff", 0, t_MRleap, handHorizontalPlaneOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "handHorizontalPlaneOnOff", 0, "onoff", "HandPlane");
	CLASS_ATTR_SAVE(c,"handHorizontalPlaneOnOff", 0);
    /*********************************************************************/
    /*****************************Pointalbes******************************/
    CLASS_ATTR_LONG(c, "toolMainOnOff", 0, t_MRleap, toolMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolMainOnOff", 0, "onoff", "ToolMain");
	CLASS_ATTR_SAVE(c,"toolMainOnOff", 0);

    CLASS_ATTR_LONG(c, "fingerMainOnOff", 0, t_MRleap, fingerMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerMainOnOff", 0, "onoff", "FingerMain");
	CLASS_ATTR_SAVE(c,"fingerMainOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "toolDirectionOnOff", 0, t_MRleap, toolDirectionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolDirectionOnOff", 0, "onoff", "ToolDirection");
	CLASS_ATTR_SAVE(c,"toolDirectionOnOff", 0);

    CLASS_ATTR_LONG(c, "fingerDirectionOnOff", 0, t_MRleap, fingerDirectionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerDirectionOnOff", 0, "onoff", "FingerDirection");
	CLASS_ATTR_SAVE(c,"fingerDirectionOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "toolTipPositionOnOff", 0, t_MRleap, toolTipPositionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolTipPositionOnOff", 0, "onoff", "ToolTip");
	CLASS_ATTR_SAVE(c,"toolTipPositionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "fingerTipPositionOnOff", 0, t_MRleap, fingerTipPositionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerTipPositionOnOff", 0, "onoff", "FingerTip");
	CLASS_ATTR_SAVE(c,"fingerTipPositionOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "toolTipNormOnOff", 0, t_MRleap, toolTipNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolTipNormOnOff", 0, "onoff", "ToolTipNorm");
	CLASS_ATTR_SAVE(c,"toolTipNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "fingerTipNormOnOff", 0, t_MRleap, fingerTipNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerTipNormOnOff", 0, "onoff", "FingerTipNorm");
	CLASS_ATTR_SAVE(c,"fingerTipNormOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "toolTipVelocityOnOff", 0, t_MRleap, toolTipVelocityOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolTipVelocityOnOff", 0, "onoff", "ToolTipVel");
	CLASS_ATTR_SAVE(c,"toolTipVelocityOnOff", 0);

    CLASS_ATTR_LONG(c, "fingerTipVelocityOnOff", 0, t_MRleap, fingerTipVelocityOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerTipVelocityOnOff", 0, "onoff", "FingerTipVel ");
	CLASS_ATTR_SAVE(c,"fingerTipVelocityOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "toolDimensionOnOff", 0, t_MRleap, toolDimensionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolDimensionOnOff", 0, "onoff", "ToolDim");
	CLASS_ATTR_SAVE(c,"toolDimensionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "fingerDimensionOnOff", 0, t_MRleap, fingerDimensionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerDimensionOnOff", 0, "onoff", "FingerDim ");
	CLASS_ATTR_SAVE(c,"fingerDimensionOnOff", 0);
    //////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "toolTouchZoneOnOff", 0, t_MRleap, toolTouchZoneOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "toolTouchZoneOnOff", 0, "onoff", "ToolTouch");
	CLASS_ATTR_SAVE(c,"toolTouchZoneOnOff", 0);
    
    CLASS_ATTR_LONG(c, "fingerTouchZoneOnOff", 0, t_MRleap, fingerTouchZoneOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "fingerTouchZoneOnOff", 0, "onoff", "FingerTouch ");
	CLASS_ATTR_SAVE(c,"fingerTouchZoneOnOff", 0);
    /*********************************************************************/
    /*************************Gesture enable******************************/
    CLASS_ATTR_LONG(c, "gestureCircleOnOff", 0, t_MRleap, gestureCircleOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureCircleOnOff", 0, "onoff", "CircleGesture");
    CLASS_ATTR_ACCESSORS(c, "gestureCircleOnOff", NULL, MRleap_gestureCircleOnOff_set);
	CLASS_ATTR_SAVE(c,"gestureCircleOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureKeyTapOnOff", 0, t_MRleap, gestureKeyTapOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureKeyTapOnOff", 0, "onoff", "KeyTapGesture");
    CLASS_ATTR_ACCESSORS(c, "gestureKeyTapOnOff", NULL, MRleap_gestureKeyTapOnOff_set);
	CLASS_ATTR_SAVE(c,"gestureKeyTapOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureScreenTapOnOff", 0, t_MRleap, gestureScreenTapOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureScreenTapOnOff", 0, "onoff", "ScreenTapGesture");
    CLASS_ATTR_ACCESSORS(c, "gestureScreenTapOnOff", NULL, MRleap_gestureScreenTapOnOff_set);
	CLASS_ATTR_SAVE(c,"gestureScreenTapOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureSwipeOnOff", 0, t_MRleap, gestureSwipeOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureSwipeOnOff", 0, "onoff", "SwipeGesture");
    CLASS_ATTR_ACCESSORS(c, "gestureSwipeOnOff", NULL, MRleap_gestureSwipeOnOff_set);
	CLASS_ATTR_SAVE(c,"gestureSwipeOnOff", 0);
    /********************************Gestures*************************************/
    CLASS_ATTR_LONG(c, "gestureCircleMainOnOff", 0, t_MRleap, gestureCircleMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureCircleMainOnOff", 0, "onoff", "CircleMain ");
	CLASS_ATTR_SAVE(c,"gestureCircleMainOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureCircleCenterOnOff", 0, t_MRleap, gestureCircleCenterOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureCircleCenterOnOff", 0, "onoff", "CircleCenter ");
	CLASS_ATTR_SAVE(c,"gestureCircleCenterOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureCircleCenterNormOnOff", 0, t_MRleap, gestureCircleCenterNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureCircleCenterNormOnOff", 0, "onoff", "CircleCenterNorm");
	CLASS_ATTR_SAVE(c,"gestureCircleCenterNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureCircleDataOnOff", 0, t_MRleap, gestureCircleDataOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureCircleDataOnOff", 0, "onoff", "CircleData");
	CLASS_ATTR_SAVE(c,"gestureCircleDataOnOff", 0);
    /////////////////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "gestureSwipeMainOnOff", 0, t_MRleap, gestureSwipeMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureSwipeMainOnOff", 0, "onoff", "SwipeMain ");
	CLASS_ATTR_SAVE(c,"gestureSwipeMainOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureSwipePositionOnOff", 0, t_MRleap, gestureSwipePositionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureSwipePositionOnOff", 0, "onoff", "SwipePosition ");
	CLASS_ATTR_SAVE(c,"gestureSwipePositionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureSwipePositionNormOnOff", 0, t_MRleap, gestureSwipePositionNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureSwipePositionNormOnOff", 0, "onoff", "SwipePosNorm");
	CLASS_ATTR_SAVE(c,"gestureSwipePositionNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureSwipeDirectionOnOff", 0, t_MRleap, gestureSwipeDirectionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureSwipeDirectionOnOff", 0, "onoff", "SwipeDirection");
	CLASS_ATTR_SAVE(c,"gestureSwipeDirectionOnOff", 0);
    /////////////////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "gestureKeyTapMainOnOff", 0, t_MRleap, gestureKeyTapMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureKeyTapMainOnOff", 0, "onoff", "KeyTapMain ");
	CLASS_ATTR_SAVE(c,"gestureKeyTapMainOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureKeyTapPositionOnOff", 0, t_MRleap, gestureKeyTapPositionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureKeyTapPositionOnOff", 0, "onoff", "KeyTapPosition ");
	CLASS_ATTR_SAVE(c,"gestureKeyTapPositionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureKeyTapPositionNormOnOff", 0, t_MRleap, gestureKeyTapPositionNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureKeyTapPositionNormOnOff", 0, "onoff", "KeyTapPosNorm");
	CLASS_ATTR_SAVE(c,"gestureKeyTapPositionNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureKeyTapDirectionOnOff", 0, t_MRleap, gestureKeyTapDirectionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureKeyTapDirectionOnOff", 0, "onoff", "KeyTapDirection");
	CLASS_ATTR_SAVE(c,"gestureKeyTapDirectionOnOff", 0);
    /////////////////////////////////////////////////////////////////////////////////
    CLASS_ATTR_LONG(c, "gestureScreenTapMainOnOff", 0, t_MRleap, gestureScreenTapMainOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureScreenTapMainOnOff", 0, "onoff", "ScreenTapMain ");
	CLASS_ATTR_SAVE(c,"gestureScreenTapMainOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureScreenTapPositionOnOff", 0, t_MRleap, gestureScreenTapPositionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureScreenTapPositionOnOff", 0, "onoff", "ScreenTapPosition ");
	CLASS_ATTR_SAVE(c,"gestureScreenTapPositionOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureScreenTapPositionNormOnOff", 0, t_MRleap, gestureScreenTapPositionNormOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureScreenTapPositionNormOnOff", 0, "onoff", "ScreenTapPosNorm");
	CLASS_ATTR_SAVE(c,"gestureScreenTapPositionNormOnOff", 0);
    
    CLASS_ATTR_LONG(c, "gestureScreenTapDirectionOnOff", 0, t_MRleap, gestureScreenTapDirectionOnOff);
	CLASS_ATTR_STYLE_LABEL(c, "gestureScreenTapDirectionOnOff", 0, "onoff", "ScreenTapDirection");
	CLASS_ATTR_SAVE(c,"gestureScreenTapDirectionOnOff", 0);
    /*****************************Gesture info****************************/
    /*circleMinRadius;
     float               circleMinArc;
     float               swipeMinLength;
     float               swipeMinVelocity;
     float               keyTapMinDownVelocity;
     float               keyTapHistorySeconds;
     float               keyTapMinDistance;
     float               screenTapMinForwardVelocity;
     float               screenTapHistorySeconds;
     float               screenTapMinDistance;
     */
    CLASS_ATTR_LONG(c, "circleMinArc", 0, t_MRleap, circleMinArc);
	CLASS_ATTR_LABEL(c, "circleMinArc", 0, "CircleMinArc");
    CLASS_ATTR_ACCESSORS(c, "circleMinArc", MRleap_gestureCircleMinArc_get, MRleap_gestureCircleMinArc_set);
	CLASS_ATTR_SAVE(c,"circleMinArc", 0);
    /*********************************************************************
    ////subgroups in inspectro
    CLASS_ATTR_CATEGORY(c, "frameHist",                         0, "Global");
    CLASS_ATTR_CATEGORY(c, "allDegOnOff",                       0, "Global");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_CATEGORY(c, "frameMainOnOff",                    0, "Frame");
    CLASS_ATTR_CATEGORY(c, "frameRotOnOff",                     0, "Frame");
    CLASS_ATTR_CATEGORY(c, "frameRotRawOnOff",                  0, "Frame");
    CLASS_ATTR_CATEGORY(c, "frameRotProb",                      0, "Frame");
    CLASS_ATTR_CATEGORY(c, "frameScaleOnOff",                   0, "Frame");
    CLASS_ATTR_CATEGORY(c, "frameScaleRawOnOff",                0, "Frame");
    CLASS_ATTR_CATEGORY(c, "frameScaleProb",                    0, "Frame");
    CLASS_ATTR_CATEGORY(c, "frameTranslationOnOff",             0, "Frame");
    CLASS_ATTR_CATEGORY(c, "frameTranslationRawOnOff",          0, "Frame");
    CLASS_ATTR_CATEGORY(c, "frameTranslationNormOnOff",         0, "Frame");
    CLASS_ATTR_CATEGORY(c, "frameTranslationProb",              0, "Frame");
    CLASS_ATTR_CATEGORY(c, "frameRotationMatrixOnOff",          0, "Frame");
    CLASS_ATTR_CATEGORY(c, "rightmostOnOff",                    0, "Frame");
    CLASS_ATTR_CATEGORY(c, "leftmostOnOff",                     0, "Frame");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_CATEGORY(c, "handMainOnOff",                     0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handSphereOnOff",                   0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handSphereNormOnOff",               0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handPositionOnOff",                 0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handPositionNormOnOff",             0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handPositionStabilizationOnOff",    0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handPositionVelocityOnOff",         0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handRotationOnOff",                 0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handRotationRawOnOff",              0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handRotationProb",                  0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handScaleOnOff",                    0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handScaleRawOnOff",                 0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handScaleProb",                     0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handTranslationOnOff",              0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handTranslationRawOnOff",           0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handTranslationNormOnOff",          0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handTranslationProb",               0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handRotationMatrixOnOff",           0, "Hand");
    CLASS_ATTR_CATEGORY(c, "handHorizontalPlaneOnOff",          0, "Hand");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_CATEGORY(c, "toolMainOnOff",                     0, "Tool");
    CLASS_ATTR_CATEGORY(c, "fingerMainOnOff",                   0, "Finger");
    CLASS_ATTR_CATEGORY(c, "toolDirectionOnOff",                0, "Tool");
    CLASS_ATTR_CATEGORY(c, "fingerDirectionOnOff",              0, "Finger");
    CLASS_ATTR_CATEGORY(c, "toolTipPositionOnOff",              0, "Tool");
    CLASS_ATTR_CATEGORY(c, "fingerTipPositionOnOff",            0, "Finger");
    CLASS_ATTR_CATEGORY(c, "toolTipNormOnOff",                  0, "Tool");
    CLASS_ATTR_CATEGORY(c, "fingerTipNormOnOff",                0, "Finger");
    CLASS_ATTR_CATEGORY(c, "toolTipVelocityOnOff",              0, "Tool");
    CLASS_ATTR_CATEGORY(c, "fingerTipVelocityOnOff",            0, "Finger");
    CLASS_ATTR_CATEGORY(c, "toolDimensionOnOff",                0, "Tool");
    CLASS_ATTR_CATEGORY(c, "fingerDimensionOnOff",              0, "Finger");
    CLASS_ATTR_CATEGORY(c, "toolTouchZoneOnOff",                0, "Tool");
    CLASS_ATTR_CATEGORY(c, "fingerTouchZoneOnOff",              0, "Finger");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_CATEGORY(c, "gestureCircleOnOff",                0, "Gesture");
    CLASS_ATTR_CATEGORY(c, "gestureKeyTapOnOff",                0, "Gesture");
    CLASS_ATTR_CATEGORY(c, "gestureScreenTapOnOff",             0, "Gesture");
    CLASS_ATTR_CATEGORY(c, "gestureSwipeOnOff",                 0, "Gesture");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_CATEGORY(c, "gestureCircleMainOnOff",            0, "Circle");
    CLASS_ATTR_CATEGORY(c, "gestureCircleCenterOnOff",          0, "Circle");
    CLASS_ATTR_CATEGORY(c, "gestureCircleCenterNormOnOff",      0, "Circle");
    CLASS_ATTR_CATEGORY(c, "gestureCircleDataOnOff",            0, "Circle");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_CATEGORY(c, "gestureSwipeMainOnOff",             0, "Swipe");
    CLASS_ATTR_CATEGORY(c, "gestureSwipePositionOnOff",         0, "Swipe");
    CLASS_ATTR_CATEGORY(c, "gestureSwipePositionNormOnOff",     0, "Swipe");
    CLASS_ATTR_CATEGORY(c, "gestureSwipeDirectionOnOff",        0, "Swipe");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_CATEGORY(c, "gestureKeyTapMainOnOff",            0, "KeyTap");
    CLASS_ATTR_CATEGORY(c, "gestureKeyTapPositionOnOff",        0, "KeyTap");
    CLASS_ATTR_CATEGORY(c, "gestureKeyTapPositionNormOnOff",    0, "KeyTap");
    CLASS_ATTR_CATEGORY(c, "gestureKeyTapDirectionOnOff",       0, "KeyTap");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_CATEGORY(c, "gestureScreenTapMainOnOff",            0, "ScreenTap");
    CLASS_ATTR_CATEGORY(c, "gestureScreenTapPositionOnOff",        0, "ScreenTap");
    CLASS_ATTR_CATEGORY(c, "gestureScreenTapPositionNormOnOff",    0, "ScreenTap");
    CLASS_ATTR_CATEGORY(c, "gestureScreenTapDirectionOnOff",       0, "ScreenTap");
    /*********************************************************************/
    //order of attributes
    CLASS_ATTR_ORDER(c, "frameHist",                            0, "1");
    CLASS_ATTR_ORDER(c, "allDegOnOff",                          0, "2");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "frameMainOnOff",                       0, "3");
    CLASS_ATTR_ORDER(c, "frameRotOnOff",                        0, "4");
    CLASS_ATTR_ORDER(c, "frameRotRawOnOff",                     0, "5");
    CLASS_ATTR_ORDER(c, "frameRotProb",                         0, "6");
    CLASS_ATTR_ORDER(c, "frameScaleOnOff",                      0, "7");
    CLASS_ATTR_ORDER(c, "frameScaleRawOnOff",                   0, "8");
    CLASS_ATTR_ORDER(c, "frameScaleProb",                       0, "9");
    CLASS_ATTR_ORDER(c, "frameTranslationOnOff",                0, "10");
    CLASS_ATTR_ORDER(c, "frameTranslationRawOnOff",             0, "11");
    CLASS_ATTR_ORDER(c, "frameTranslationNormOnOff",            0, "12");
    CLASS_ATTR_ORDER(c, "frameTranslationProb",                 0, "13");
    CLASS_ATTR_ORDER(c, "frameRotationMatrixOnOff",             0, "14");
//    CLASS_ATTR_ORDER(c, "rightmostOnOff",                       0, "14");
//    CLASS_ATTR_ORDER(c, "leftmostOnOff",                        0, "14");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "handMainOnOff",                        0, "15");
    CLASS_ATTR_ORDER(c, "handSphereOnOff",                      0, "16");
    CLASS_ATTR_ORDER(c, "handSphereNormOnOff",                  0, "17");
    CLASS_ATTR_ORDER(c, "handPositionOnOff",                    0, "18");
    CLASS_ATTR_ORDER(c, "handPositionNormOnOff",                0, "19");
    CLASS_ATTR_ORDER(c, "handPositionStabilizationOnOff",       0, "20");
    CLASS_ATTR_ORDER(c, "handPositionVelocityOnOff",            0, "21");
    CLASS_ATTR_ORDER(c, "handRotationOnOff",                    0, "22");
    CLASS_ATTR_ORDER(c, "handRotationRawOnOff",                 0, "23");
    CLASS_ATTR_ORDER(c, "handRotationProb",                     0, "24");
    CLASS_ATTR_ORDER(c, "handScaleOnOff",                       0, "25");
    CLASS_ATTR_ORDER(c, "handScaleRawOnOff",                    0, "26");
    CLASS_ATTR_ORDER(c, "handScaleProb",                        0, "27");
    CLASS_ATTR_ORDER(c, "handTranslationOnOff",                 0, "28");
    CLASS_ATTR_ORDER(c, "handTranslationRawOnOff",              0, "29");
    CLASS_ATTR_ORDER(c, "handTranslationNormOnOff",             0, "30");
    CLASS_ATTR_ORDER(c, "handTranslationProb",                  0, "31");
    CLASS_ATTR_ORDER(c, "handRotationMatrixOnOff",              0, "32");
    CLASS_ATTR_ORDER(c, "handHorizontalPlaneOnOff",             0, "33");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "fingerMainOnOff",                      0, "34");
    CLASS_ATTR_ORDER(c, "fingerDirectionOnOff",                 0, "35");
    CLASS_ATTR_ORDER(c, "fingerTipPositionOnOff",               0, "36");
    CLASS_ATTR_ORDER(c, "fingerTipNormOnOff",                   0, "37");
    CLASS_ATTR_ORDER(c, "fingerTipVelocityOnOff",               0, "38");
    CLASS_ATTR_ORDER(c, "fingerDimensionOnOff",                 0, "39");
    CLASS_ATTR_ORDER(c, "fingerTouchZoneOnOff",                 0, "40");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "toolMainOnOff",                        0, "41");
    CLASS_ATTR_ORDER(c, "toolDirectionOnOff",                   0, "42");
    CLASS_ATTR_ORDER(c, "toolTipPositionOnOff",                 0, "43");
    CLASS_ATTR_ORDER(c, "toolTipNormOnOff",                     0, "44");
    CLASS_ATTR_ORDER(c, "toolTipVelocityOnOff",                 0, "45");
    CLASS_ATTR_ORDER(c, "toolDimensionOnOff",                   0, "46");
    CLASS_ATTR_ORDER(c, "toolTouchZoneOnOff",                   0, "47");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "gestureCircleOnOff",                   0, "48");
    CLASS_ATTR_ORDER(c, "gestureKeyTapOnOff",                   0, "49");
    CLASS_ATTR_ORDER(c, "gestureScreenTapOnOff",                0, "50");
    CLASS_ATTR_ORDER(c, "gestureSwipeOnOff",                    0, "51");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "gestureCircleMainOnOff",               0, "52");
    CLASS_ATTR_ORDER(c, "gestureCircleCenterOnOff",             0, "53");
    CLASS_ATTR_ORDER(c, "gestureCircleCenterNormOnOff",         0, "54");
    CLASS_ATTR_ORDER(c, "gestureCircleDataOnOff",               0, "55");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "gestureSwipeMainOnOff",                0, "56");
    CLASS_ATTR_ORDER(c, "gestureSwipePositionOnOff",            0, "57");
    CLASS_ATTR_ORDER(c, "gestureSwipePositionNormOnOff",        0, "58");
    CLASS_ATTR_ORDER(c, "gestureSwipeDirectionOnOff",           0, "59");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "gestureKeyTapMainOnOff",               0, "60");
    CLASS_ATTR_ORDER(c, "gestureKeyTapPositionOnOff",           0, "61");
    CLASS_ATTR_ORDER(c, "gestureKeyTapPositionNormOnOff",       0, "62");
    CLASS_ATTR_ORDER(c, "gestureKeyTapDirectionOnOff",          0, "63");
    ///////////////////////////////////////////////////////////////////////
    CLASS_ATTR_ORDER(c, "gestureScreenTapMainOnOff",            0, "64");
    CLASS_ATTR_ORDER(c, "gestureScreenTapPositionOnOff",        0, "65");
    CLASS_ATTR_ORDER(c, "gestureScreenTapPositionNormOnOff",    0, "66");
    CLASS_ATTR_ORDER(c, "gestureScreenTapDirectionOnOff",       0, "67");
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
    
    object_post((t_object *)x, "MRleap 0.2.1 for The Leap 1.0.9");
    
    x->prevFrameID                      = 0;
    x->frameHist                        = 1;
    x->allDegOnOff                      = 0; //radians by default
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
    x->rightmostOnOff                   = false;
    x->leftmostOnOff                    = false;
    x->rightmostID                      = -1;
    x->leftmostID                       = -1;
    x->prevRightmostID                  = -1;
    x->prevLeftmostID                   = -1;
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
    
    
    
    ////////gesture attr default
    //////////gesture attr
    x->circleMinRadius                  = 5.;
    x->circleMinArc                     = 1.5 * Leap::PI;
    x->swipeMinLength                   = 150;
    x->swipeMinVelocity                 = 1000;
    x->keyTapMinDownVelocity            = 50;
    x->keyTapHistorySeconds             = 0.1;
    x->keyTapMinDistance                = 3;
    x->screenTapMinForwardVelocity      = 50;
    x->screenTapHistorySeconds          = 0.1;
    x->screenTapMinDistance             = 5;
    
    /////////outlets
    x->outletStart      = outlet_new((t_MRleap *)x, NULL);
    x->outletFrame      = outlet_new((t_MRleap *)x, NULL);
    x->outletHands      = outlet_new((t_MRleap *)x, NULL);
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
    

    x->RIGHT = gensym("rightmost");
    x->LEFT  = gensym("leftmost");
    x->HAND  = x->LEFT;
    x->OTHER = gensym("other");
    
    // call this after initializing defaults to overwrite with saved attr
    attr_args_process(x, argc, argv);
    
    //    Leap::Device testDevice;
    
    
	return (x);
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
                sprintf(s, "Hands");
                break;
            case 4:
                sprintf(s, "Frame");
                break;
            case 5:
                sprintf(s, "frame start/end [1/0]");
                break;
        }
	}
}
/************************************/
void MRleap_free(t_MRleap *x)
{
	if (x->leap)
		delete (Leap::Controller *)(x->leap);
}
/************************************/
void MRleap_bang(t_MRleap *x)
{
    if( x->leap->isConnected()) //controller is a Controller object
    {
        
        const Leap::Frame frame = x->leap->frame();
        
        if (frame.isValid())    {
            
            x->curFrameID   = frame.id();
            
            // ignore the same frame
            if (x->curFrameID == x->prevFrameID) {
                
                return;
            }
            
            outlet_int(x->outletStart, (long)1);
            
            /*************Frame info****************/
            MRleap_getFrameData(x, frame);
            
            
            /***************Hand info****************/
            if (!frame.hands().isEmpty()) {
                
                //Process hands...
                MRleap_getHandData(x, frame);
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
        x->leap = new Leap::Controller;
        
/* 
    //for testing output when I have no leap on me...
 
        t_atom handMain[6];
        
        atom_setsym(handMain,       gensym("handMain"));
        atom_setlong(handMain+1,    55.5);
        atom_setlong(handMain+2,    rand());
        atom_setlong(handMain+3,    rand() * 1000); //sec -> ms
        atom_setlong(handMain+4,    22.2);
        atom_setlong(handMain+5,    rand() + 22);
        
        
        outlet_anything(x->outletHands, x->HAND, 6, handMain);
 */
    }
}
/************************************/
void MRleap_getFrameData(t_MRleap *x, Leap::Frame frame)
{
    
    const Leap::HandList    hands       = frame.hands();
    const int               numHands    = hands.count(); //what's the max?? (have seen up to 5 so far)
    float                   fps         = frame.currentFramesPerSecond();
    
    if(x->frameMainOnOff)    {
        
        //leftmost()
        //righmost()
        //frontmost()
        /***************Frame info*********************/
        //output frame info orientation translation etc....
        
        //              float framePeriod = frame.timestamp() - x->leap->frame(1).timestamp();
        
        
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
            
            atom_setfloat(frameRotationMatrix + i, array.m_array[i]);
        }
        
        outlet_anything(x->outletFrame, x->s_rotationMatrix, 10, frameRotationMatrix);
    }
    
    
    /////////////which hand??////////////////////
    Leap::HandList handList = frame.hands();

    if (!handList.isEmpty())   {
        
        Leap::Hand   leftHand = frame.hands().leftmost();
        
        if (leftHand.isValid()) {
            
            
             x->leftmostID = leftHand.id();
        }
        
        Leap::Hand   rightHand = frame.hands().rightmost();
        
        if (rightHand.isValid()) {
                
            x->rightmostID = rightHand.id();
        }
        
        //sort the rest of the hands??
        
        if (x->rightmostID >= 0 && x->leftmostID >= 0)  {
            
            if (x->rightmostID == x->leftmostID && x->rightmostID == x->prevLeftmostID) {
            //if hand ids are the same (one hand detected), and the current right hand is the same as the previous left hand
            // -> the right hand has been removed
            
                x->rightmostID = -1;
            }
            else if (x->rightmostID == x->leftmostID && x->leftmostID == x->prevRightmostID) {
            
                x->leftmostID = -1;
            }
        }
/*        else if (x->prevRightmostID < 0 && x->prevLeftmostID < 0){
         
            x->rightmostID = rightHand.id();
            x->leftmostID  = -1;
            
        }
 */
    }
}
/*
 
 Leap::Matrix        mtxTotalMotionRotation;
 Leap::Vector        vecTotalMotionTranslation;
 float               fTotalMotionScale;
 
 //Update the translation in the current reference frame
 if (( m_uiFlags & kFlag_Translate ) && bShouldTranslate)
 {
 m_vTotalMotionTranslation += m_mtxTotalMotionRotation.rigidInverse().transformDirection(frame.translation(m_lastFrame));
 }
 
 //Update the rotation
 if (( m_uiFlags & kFlag_Rotate ) && bShouldRotate)
 {
 m_mtxTotalMotionRotation = frame.rotationMatrix(m_lastFrame) * m_mtxTotalMotionRotation; // Left multiply, relative to last frame
 }
 
 //Update the scale
 if (( m_uiFlags & kFlag_Scale ) && bShouldScale)
 {
 m_fTotalMotionScale = LeapUtil::Clamp(  m_fTotalMotionScale * frame.scaleFactor(m_lastFrame),
 kfMinScale,
 kfMaxScale );
 }
 */
/************************************/
void MRleap_getHandData(t_MRleap *x, Leap::Frame frame)
{
    
    Leap::HandList          hands       = frame.hands();
    const int               numHands    = hands.count(); //what's the max?? (have seen up to 5 so far
    
    for(int i = 0; i < numHands; ++i)  {
        
        const Leap::Hand hand = hands[i];
        
        if (hand.isValid()) {
            
            MRleap_assignHandID(x, hand);
            
            long handID = hand.id();
            
            if(x->handMainOnOff)    {
          
                t_atom handMain[6];
                
                atom_setsym(handMain,       x->s_handMain);
                atom_setlong(handMain+1,    handID);
                atom_setlong(handMain+2,    x->curFrameID);
                atom_setlong(handMain+3,    hand.timeVisible() * 1000); //sec -> ms
                atom_setlong(handMain+4,    hand.fingers().count());
                atom_setlong(handMain+5,    hand.tools().count());
                
                outlet_anything(x->outletHands, x->HAND, 6, handMain);
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

                t_atom handPos[6];
 
                atom_setsym(handPos,        x->s_position);
                atom_setlong(handPos+1,     handID);
                atom_setlong(handPos+2,     x->curFrameID);
                atom_setfloat(handPos+3,    pos[0]);
                atom_setfloat(handPos+4,    pos[1]);
                atom_setfloat(handPos+5,    pos[2]);
                
                outlet_anything(x->outletHands, x->HAND, 6, handPos);
              
            }
            //////////////////////////////
            
            if (x->handPositionVelocityOnOff)   {
         
                t_atom handVel[6];
                
                atom_setsym(handVel,        x->s_velocity);
                atom_setlong(handVel+1,     handID);
                atom_setlong(handVel+2,     x->curFrameID);
                atom_setfloat(handVel+3,    hand.palmVelocity()[0]);
                atom_setfloat(handVel+4,    hand.palmVelocity()[1]);
                atom_setfloat(handVel+5,    hand.palmVelocity()[2]);
            
                outlet_anything(x->outletHands, x->HAND, 6, handVel);
            }
            
            ////////sphere////////////////
            if (x->handSphereOnOff) {
                
                Leap::Vector sphere = MRleap_normalizeVec(x, frame, hand.sphereCenter(), x->handSphereNorm);
                
                t_atom handSphere[7];
                
                atom_setsym(handSphere,         x->s_sphere);
                atom_setlong(handSphere+1,      handID);
                atom_setlong(handSphere+2,      x->curFrameID);
                atom_setfloat(handSphere+3,     hand.sphereRadius());
                atom_setfloat(handSphere+4,     sphere[0]);
                atom_setfloat(handSphere+5,     sphere[1]);
                atom_setfloat(handSphere+6,     sphere[2]);
                
                outlet_anything(x->outletHands, x->HAND, 7, handSphere);
            }
            ///////////rotation////////////////////
            float rotationProb = hand.rotationProbability(x->leap->frame(x->frameHist));
            
            if (rotationProb >= x->handRotationProb && x->handRotationOnOff == true)  {
                
                t_atom handRotCooked[6];
                
                atom_setsym(handRotCooked,       x->s_rotation);
                atom_setlong(handRotCooked+1,    handID);
                atom_setlong(handRotCooked+2,    x->curFrameID);
                
                float whichAxis = MRleap_biggestRotAxis(x, hand.rotationAxis(x->leap->frame(x->frameHist)));
                float rotationAngle  = -1;
                
                if (whichAxis == X_AXIS)    {
                    
                    atom_setsym(handRotCooked+4,          x->s_x);
                    rotationAngle = hand.rotationAngle(x->leap->frame(x->frameHist), Leap::Vector::xAxis());
                }
                else if (whichAxis == Y_AXIS)    {
                    
                    atom_setsym(handRotCooked+4,          x->s_y);
                    rotationAngle = hand.rotationAngle(x->leap->frame(x->frameHist), Leap::Vector::yAxis());
                }
                else if (whichAxis == Z_AXIS)    {
                    
                    atom_setsym(handRotCooked+4,          x->s_z);
                    rotationAngle = hand.rotationAngle(x->leap->frame(x->frameHist), Leap::Vector::zAxis());
                }
                
                rotationAngle = rotationAngle * MRleap_RadDeg(x);
                
                atom_setfloat(handRotCooked+3,        rotationProb);
                atom_setfloat(handRotCooked+5,        rotationAngle);
                
                outlet_anything(x->outletHands, x->HAND, 6, handRotCooked);
            }
            
            /////////////////////////////////
            if (x->handRotationRawOnOff)    {
                
                t_atom handRotRaw[10];
                
                atom_setsym(handRotRaw,          x->s_rotationRaw);
                atom_setlong(handRotRaw+1,       handID);
                atom_setlong(handRotRaw+2,       x->curFrameID);
                atom_setfloat(handRotRaw+3,      rotationProb);
                atom_setfloat(handRotRaw+4,      hand.rotationAxis(x->leap->frame(1))[0]);
                atom_setfloat(handRotRaw+5,      hand.rotationAxis(x->leap->frame(1))[1]);
                atom_setfloat(handRotRaw+6,      hand.rotationAxis(x->leap->frame(1))[2]);
                atom_setfloat(handRotRaw+7,      hand.rotationAngle(x->leap->frame(1), Leap::Vector::xAxis()) * MRleap_RadDeg(x));
                atom_setfloat(handRotRaw+8,      hand.rotationAngle(x->leap->frame(1), Leap::Vector::yAxis()) * MRleap_RadDeg(x));
                atom_setfloat(handRotRaw+9,      hand.rotationAngle(x->leap->frame(1), Leap::Vector::zAxis()) * MRleap_RadDeg(x));
                
                
                outlet_anything(x->outletHands, x->HAND, 10, handRotRaw);
            }
            /////////////scale/////////////////////
            float scaleProb = hand.scaleProbability(x->leap->frame(x->frameHist));
            
            if (x->handScaleRawOnOff)  {
                
                t_atom handScaleRaw[5];
                
                atom_setsym(handScaleRaw,           x->s_scaleRaw);
                atom_setlong(handScaleRaw+1,        handID);
                atom_setlong(handScaleRaw+2,        x->curFrameID);
                atom_setfloat(handScaleRaw+3,       scaleProb);
                atom_setfloat(handScaleRaw+4,       hand.scaleFactor(x->leap->frame(x->frameHist)));
                
                outlet_anything(x->outletHands, x->HAND, 5, handScaleRaw);
            }
            
            ///////////////////////////////////
            if (x->handScaleOnOff && scaleProb >= x->handScaleProb)  {
                
                t_atom handScale[5];
                
                atom_setsym(handScale,      x->s_scale);
                atom_setlong(handScale+1,   handID);
                atom_setlong(handScale+2,   x->curFrameID);
                atom_setfloat(handScale+3,  scaleProb);
                atom_setfloat(handScale+4,  hand.scaleFactor(x->leap->frame(x->frameHist)));
                
                outlet_anything(x->outletHands, x->HAND, 5, handScale);
            }
            ////////////////////////////////
            
            /////////////translation/////////////////////
            float translationProb = hand.translationProbability(x->leap->frame(x->frameHist));
            
            
            
            if (x->handTranslationRawOnOff)    {
                
                Leap::Vector trans = MRleap_normalizeVec(x, frame, hand.translation(x->leap->frame(x->frameHist)), x->handTranslationNormOnOff);
                
                t_atom handTranslationRaw[7];
                
                atom_setsym(handTranslationRaw,         x->s_translationRaw);
                atom_setlong(handTranslationRaw+1,      handID);
                atom_setlong(handTranslationRaw+2,      x->curFrameID);
                atom_setfloat(handTranslationRaw+3,     translationProb);
                atom_setfloat(handTranslationRaw+4,     trans.x);
                atom_setfloat(handTranslationRaw+5,     trans.y);
                atom_setfloat(handTranslationRaw+6,     trans.z);
                
                outlet_anything(x->outletHands, x->HAND, 7, handTranslationRaw);
            }
            
            if (x->handTranslationOnOff && x->handTranslationProb > translationProb)  {
                
                Leap::Vector trans = MRleap_normalizeVec(x, frame, hand.translation(x->leap->frame(x->frameHist)), x->handTranslationNormOnOff);
                t_atom handTranslation[7];
                
                atom_setsym(handTranslation,        x->s_translation);
                atom_setlong(handTranslation+1,     handID);
                atom_setlong(handTranslation+2,     x->curFrameID);
                atom_setfloat(handTranslation+3,    translationProb);
                atom_setfloat(handTranslation+4,    trans.x);
                atom_setfloat(handTranslation+5,    trans.y);
                atom_setfloat(handTranslation+6,    trans.z);
                
                outlet_anything(x->outletHands, x->HAND, 7, handTranslation);
            }
            ///////////////////////////////////////////////
            ///////////////matrix/////////////////////
            
            if (x->handRotationMatrix) {
                
                Leap::FloatArray array = hand.rotationMatrix(x->leap->frame(x->frameHist)).toArray3x3();
                t_atom handRotationMatrix[12];
                
                atom_setsym(handRotationMatrix,        x->s_rotationMatrix);
                atom_setlong(handRotationMatrix+1,     handID);
                atom_setlong(handRotationMatrix+2,     x->curFrameID);
                
                for (int i = 3; i < 12; i++)    {
                    
                    atom_setfloat(handRotationMatrix + i, array.m_array[i]);
                }
                
                outlet_anything(x->outletHands, x->HAND, 12, handRotationMatrix);
            }
            
            //////////////horizontal plane////////////
            if (x->handHorizontalPlaneOnOff)    {
                
                t_atom handPlane[6];
                
                atom_setsym(handPlane,          x->s_plane);
                atom_setlong(handPlane+1,       handID);
                atom_setlong(handPlane+2,       x->curFrameID);
                atom_setfloat(handPlane+3,      hand.direction().pitch() * MRleap_RadDeg(x));
                atom_setfloat(handPlane+4,      hand.direction().yaw()   * MRleap_RadDeg(x));
                atom_setfloat(handPlane+5,      hand.palmNormal().roll() * MRleap_RadDeg(x));
                
                outlet_anything(x->outletHands, x->HAND, 6, handPlane);
            }
            
            /************************************/
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
    Leap::PointableList     pointables      = frame.pointables();
    const int               numPointables   = pointables.count(); //what's the max?? (have seen up to 5 so far
    
    for(int i = 0; i < numPointables; ++i)  {
        
        const Leap::Pointable point = pointables[i];
        
        if (point.isValid()) {
            
            if(point.isFinger())  {
                

                MRleap_assignHandID(x, point.hand());
                
                long handID     = point.hand().id();
                long pointID    = point.id();
                
                if(x->fingerMainOnOff)    {
                    
                    t_atom fingerMain[6];
                
                    atom_setsym(fingerMain,       x->s_fingerMain);
                    atom_setlong(fingerMain+1,    pointID);
                    atom_setlong(fingerMain+2,    handID);
                    atom_setlong(fingerMain+3,    x->curFrameID);
                    atom_setlong(fingerMain+4,    numPointables);
                    atom_setfloat(fingerMain+5,   point.timeVisible() * 1000);//sec -> ms
                    
                    outlet_anything(x->outletFingers, x->HAND, 6, fingerMain);
                }
                /////////////direction///////////////
                if (x->fingerDirectionOnOff) {
                    t_atom pointDirection[7];
                    
                    atom_setsym(pointDirection,         x->s_direction);
                    atom_setlong(pointDirection+1,      pointID);
                    atom_setlong(pointDirection+2,      handID);
                    atom_setlong(pointDirection+3,      x->curFrameID);
                    atom_setfloat(pointDirection+4,     point.direction().x);
                    atom_setfloat(pointDirection+5,     point.direction().y);
                    atom_setfloat(pointDirection+6,     point.direction().z);
                    
                    outlet_anything(x->outletFingers, x->HAND, 7, pointDirection);
                }
                /////////////tip///////////////
                if (x->fingerTipPositionOnOff)    {
                    
                    
                    Leap::Vector tip = MRleap_normalizeVec(x, frame, point.tipPosition(), x->fingerTipNormOnOff);
                    
                    t_atom pointTip[7];
                    
                    atom_setsym(pointTip,       x->s_tip);
                    atom_setlong(pointTip+1,    pointID);
                    atom_setlong(pointTip+2,    handID);
                    atom_setlong(pointTip+3,    x->curFrameID);
                    atom_setfloat(pointTip+4,   tip.x);
                    atom_setfloat(pointTip+5,   tip.y);
                    atom_setfloat(pointTip+6,   tip.z);
                    
                    outlet_anything(x->outletFingers, x->HAND, 7, pointTip);
                    
                }
                /////////////tip velocity///////////////
                
                if (x->fingerTipVelocityOnOff)    {
                    
                    t_atom pointVel[7];
                    
                    atom_setsym(pointVel,       x->s_tipVelocity);
                    atom_setlong(pointVel+1,    pointID);
                    atom_setlong(pointVel+2,    handID);
                    atom_setlong(pointVel+3,    x->curFrameID);
                    atom_setfloat(pointVel+4,   point.tipVelocity().x);
                    atom_setfloat(pointVel+5,   point.tipVelocity().y);
                    atom_setfloat(pointVel+6,   point.tipVelocity().z);
                    
                    outlet_anything(x->outletFingers, x->HAND, 7, pointVel);
                }
                ///////////////tip dimension///////////
                if (x->fingerDimensionOnOff)  {
                    
                    t_atom pointDim[6];
                    
                    atom_setsym(pointDim,       x->s_tipDimension);
                    atom_setlong(pointDim+1,    pointID);
                    atom_setlong(pointDim+2,    handID);
                    atom_setlong(pointDim+3,    x->curFrameID);
                    atom_setfloat(pointDim+4,   point.width());
                    atom_setfloat(pointDim+5,   point.length());
                    
                    outlet_anything(x->outletFingers, x->HAND, 6, pointDim);
                }
                if (x->fingerTouchZoneOnOff)  {
                    
                    t_atom pointTouch[6];
                    
                    atom_setsym(pointTouch,         x->s_touchZone);
                    atom_setlong(pointTouch+1,      pointID);
                    atom_setlong(pointTouch+2,      handID);
                    atom_setlong(pointTouch+3,      x->curFrameID);
                    atom_setfloat(pointTouch+4,     point.touchDistance());
                    atom_setlong(pointTouch+5,      point.touchZone());
                    
                    outlet_anything(x->outletFingers, x->HAND, 6, pointTouch);
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
    x->circleMinRadius              = circleMinRadiusDef;
    x->circleMinArc                 = circleMinArcDef;
    x->swipeMinLength               = swipeMinLengthDef;
    x->swipeMinVelocity             = swipeMinVelocityDef;
    x->keyTapMinDownVelocity        = keyTapMinDownVelocityDef;
    x->keyTapHistorySeconds         = keyTapHistorySecondsDef;
    x->keyTapMinDistance            = keyTapMinDistanceDef;
    x->screenTapMinForwardVelocity  = screenTapMinForwardVelocityDef;
    x->screenTapHistorySeconds      = screenTapHistorySecondsDef;
    x->screenTapMinDistance         = screenTapMinDistanceDef;
}
/************************************/
t_max_err MRleap_gestureCircleMinArc_set(t_MRleap *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)   {
        
        x->circleMinArc = atom_getfloat(argv) * MRleap_RadDeg(x);
        
        return MAX_ERR_NONE;
    }
    
    return MAX_ERR_GENERIC;
}
/************************************/
t_max_err MRleap_gestureCircleMinArc_get(t_MRleap *x, t_object *attr, long *argc, t_atom **argv)
{
    char alloc;
    float size = 0;
    
    atom_alloc(argc, argv, &alloc);     // allocate return atom

    
    if (x->circleMinArc)
        size = atom_getfloat(*argv) * MRleap_RadDeg(x);
        
        atom_setlong(*argv, size);
    
    
    return MAX_ERR_NONE;
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
    /*x->leftmostID and x->rightmostID are calculated at the end of each getFrameData() function
     */
    long handID = hand.id();
    
    if (handID == x->rightmostID)   {
        
        x->HAND = x->RIGHT;
    }
    else if (handID == x->leftmostID)   {
        
         x->HAND = x->LEFT;
    }
    else if (x->rightmostID >= 0 && x->leftmostID >= 0 && handID != x->rightmostID && handID != x->leftmostID)   {
        
        x->HAND = x->OTHER;
    }
    else    {
       
        
        
        
        post("which hand??");
        
        
        
        
        //might need to be smarter about this:
        // -1 is assigned to the hand that is removed -> we don't want it to show up as a x->OTHER!!!
        

        
        
//        x->HAND = x->OTHER;
    }
    
    x->prevLeftmostID  = x->leftmostID;
    x->prevRightmostID = x->rightmostID;
}
/************************************/
/************************************
 Transforming Finger Coordinates into the Hand's Frame of Reference
 
 Sometimes it is useful to obtain the coordinates of the fingers of a hand with respect to the hand's frame of reference. This lets you sort the fingers spatially and can simplify analysis of finger positions. You can create a transform matrix using the Leap Matrix class to transform the finger position and direction coordinates. The hand frame of reference can be usefully defined with the hand's direction and palm normal vectors, with the third axis defined by the cross product between the two. This puts the x-axis sideways across the hand, the z-axis pointing forward, and the y-axis parallel with the palm normal.
 Leap::Frame frame = leap.frame();
 for( int h = 0; h < frame.hands().count(); h++ )
 {
 Leap::Hand leapHand = frame.hands()[h];
 
 Leap::Vector handXBasis =  leapHand.palmNormal().cross(leapHand.direction()).normalized();
 Leap::Vector handYBasis = -leapHand.palmNormal();
 Leap::Vector handZBasis = -leapHand.direction();
 Leap::Vector handOrigin =  leapHand.palmPosition();
 Leap::Matrix handTransform = Leap::Matrix(handXBasis, handYBasis, handZBasis, handOrigin);
 handTransform = handTransform.rigidInverse();
 
 for( int f = 0; f < leapHand.fingers().count(); f++ )
 {
 Leap::Finger leapFinger = leapHand.fingers()[f];
 Leap::Vector transformedPosition = handTransform.transformPoint(leapFinger.tipPosition());
 Leap::Vector transformedDirection = handTransform.transformDirection(leapFinger.direction());
 // Do something with the transformed fingers
 }
 }
*/
/************************************/