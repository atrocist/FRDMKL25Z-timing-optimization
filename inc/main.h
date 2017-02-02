//first set of mag readings default no magnanometer
#define	X_M 0.014660765f	 //0.84 * M_PI/180;
#define	Y_M 0.374896723f	 //21.48 * M_PI/180;
#define	Z_M -0.743510261f	 //-42.6 * M_PI/180;

//second set if mag readings with roll and pitch as zero
#define X_M_1 0.296356907f				//16.98 * PI/180;
#define Y_M_1 0.168598805f				//9.66 * PI/180;
#define Z_M_1 -0.738274273f				//-42.3 * PI/180

//third set of mag readings default no magnanometer with roll and pitch statically assigned
#define	X_M_2 -0.240855436f	 //-13.8 * M_PI/180;		
#define	Y_M_2 0.282743338f	 //16.2 * M_PI/180;
#define	Z_M_2 -0.740368668f	 //-42.42 * M_PI/180;
#define X_ACC_2 3840
#define Y_ACC_2 -2080
#define Z_ACC_2 15616
//pre calculate roll and pitch
#define ROLL_2 -0.132417171f
#define PITCH_2 0.239086721f

float tilt_compensation(void);
void Init_GPIO_TRIGs(void);
float cos_approx(float x);


uint32_t trigger_1, trigger_2;
float mag_X, mag_Y, mag_Z;
float mag_X_1, mag_Y_1, mag_Z_1;

float compensated_heading;

const float a1 = 0.99940307f;
const float a2 = -0.49558072f;
const float a3 = 0.03679168f;
