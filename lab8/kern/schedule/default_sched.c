#include <defs.h>
#include <list.h>
#include <proc.h>
#include <assert.h>
#include <default_sched.h>

static void
mfq_init(struct run_queue *rq) {
     list_init(&(rq->run_list));
     rq->proc_num = 0;
}

static void
mfq_enqueue(struct run_queue **rqs, struct proc_struct *proc) {
     int level = proc->queue_level+1;
     if(level==6) level=5;//之前已经在最低队列中了，不能再降级了
     assert(list_empty(&(proc->run_link)));//确保链表不为空
     list_add_before(&(rqs[level]->run_list), &(proc->run_link));//proc插入链表
     if (proc->time_slice == 0 || proc->time_slice > rqs[level]->max_time_slice) {
          proc->time_slice = rqs[level]->max_time_slice;//赋值时间片
     }
     proc->rq = rqs[level];
     rqs[level]->proc_num ++;//增加rq的进程数量
}

static void
mfq_dequeue(struct proc_struct *proc) {
     struct run_queue* rq = proc->rq;
     assert(!list_empty(&(proc->run_link)));
     list_del_init(&(proc->run_link));//从链表中删除
     rq->proc_num --;//减少进程的数量
     proc->queue_level = proc->queue_level+1; //如果被拿出队列，则需要降级
     if(proc->queue_level==6) proc->queue_level=5;//已经是最低级了
}

static struct proc_struct *
mfq_pick_next(struct run_queue **rqs) {
     int i;
     for(i=0;i<6;++i){ //遍历每个队列，优先选出高级的队列中的进程执行
          list_entry_t *le = list_next(&(rqs[i]->run_list));
          if (le != &(rqs[i]->run_list)) {
          return le2proc(le, run_link);
          }
     }
     return NULL;
}

/*
 * stride_proc_tick works with the tick event of current process. You
 * should check whether the time slices for current process is
 * exhausted and update the proc struct ``proc''. proc->time_slice
 * denotes the time slices left for current
 * process. proc->need_resched is the flag variable for process
 * switching.
 */
static void
mfq_proc_tick(struct run_queue *rq, struct proc_struct *proc) {
     if (proc->time_slice > 0){
          proc->time_slice --;
     }
     if (proc->time_slice == 0){
          proc->need_resched=1;
     }
}

struct sched_class default_sched_class = {
     .name = "mfq_scheduler",
     .init = mfq_init,
     .enqueue = mfq_enqueue,
     .dequeue = mfq_dequeue,
     .pick_next = mfq_pick_next,
     .proc_tick = mfq_proc_tick,
};


