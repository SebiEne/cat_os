/*
 * scheduler.h
 *
 * Created: 4/6/2018 3:04:40 AM
 *  Author: sene
 */


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>
#include <list.h>
#include <semaphore.h>

#define CONFIG_SCHEDULER_TASK_COLORATION      (1)
#define CONFIG_SCHEDULER_IDLE_TASK_STACK_SIZE (512)
#define MCU_CONTEXT_SIZE                      (8)

enum task_state_e {
  READY,
  RUNNING,
  WAITING_FOR_SEM,
  HALTED
};

struct tcb_s {
  void (*entry_point)(void);
  enum task_state_e t_state;
  void *stack_ptr_base;
  void *stack_ptr_top;
  void *sp;
  void *mcu_context[MCU_CONTEXT_SIZE];
  struct list_head next_tcb;
  sem_t *waiting_tcb_sema;
} __attribute__((aligned(4)));

int sched_init(void);

int sched_create_task(void (*task_entry_point)(void), uint32_t stack_size);

void sched_run(void);

struct tcb_s *sched_get_current_task(void);

struct tcb_s *sched_get_next_task(void);

int sched_desroy(void);

void disable_int(void);

void enable_int(void);

void sched_context_switch(void);

#endif /* SCHEDULER_H_ */
