#ifdef __cplusplus  
extern "C" {
#endif
extern void nes_Init(void);
extern void nes_Step(void);
extern void nes_Start(char * filename);
extern void nes_Input(int key);
#ifdef __cplusplus    
}
#endif
