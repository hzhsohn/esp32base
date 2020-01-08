#ifndef __ZH_TIMER_GROUP__H__
#define __ZH_TIMER_GROUP__H__

#ifdef __cplusplus
extern "C"{
#endif

typedef void PF_TIMER_GROUP(int timerID);

void timer_group_init(PF_TIMER_GROUP*pf,float time0_sec,float time1_sec);
void timer_evt_task(void *arg);

#ifdef __cplusplus
}
#endif
#endif