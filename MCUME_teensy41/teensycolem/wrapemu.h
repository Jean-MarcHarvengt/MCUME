#ifdef __cplusplus  
extern "C" {
#endif
extern void coc_Init(void);
extern void coc_Start(char * CartName);
extern void coc_Step(void);
extern void coc_Stop(void);
extern void coc_Input(int click);
#ifdef __cplusplus
}
#endif
