
#ifdef __cplusplus
extern "C" {
#endif

struct CSteadyTimer;
typedef struct CSteadyTimer CSteadyTimer;
CSteadyTimer * create_steadytimer();
void free_steadytimer(CSteadyTimer * p);
double get_microseconds_steadytimer(CSteadyTimer * p);

#ifdef __cplusplus
}
#endif

