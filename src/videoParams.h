enum camera
{
    LEFT = 0,
    RIGHT,
    MAIN,
    NUM_CAMERAS
};

typedef struct
{
   float alphas[NUM_CAMERAS];
   float betas[NUM_CAMERAS];
} VideoParams;
