
void enqueue_nonRT(process_t* proc){
	if(nonrt_process_queue == NULL){
		nonrt_process_queue = proc;
	}
	else{
		process_t * tmp;
		tmp = nonrt_process_queue;
		while (tmp->next_state != NULL) {
			// while there are more elements in the list
			tmp = tmp->next_state;
		}
		// now tmp is the last element in the queue
		// add new_state to end of queue
		tmp->next_state = proc;
		proc->next_state = NULL;
	}
}

void enqueue_RT_waiting(process_t* proc){
	if(rt_waiting_queue == NULL){
		rt_waiting_queue = proc;
		proc->next_state = NULL;
	}
	else{
        //ROUND ROBIN EXECUTION
        int msec = 0, trigger = 10; /* 10ms */
        clock_t before = clock();
        process_t* tmp = rt_waiting_queue;
        while(tmp->next_state != NULL){
            tmp = tmp->next_state;
        }
        proc = tmp->next_state;

        do {
            clock_t difference = clock() - before;
            msec = difference * 1000 / CLOCKS_PER_SEC;
            iterations++;
        } while ( msec < trigger );

        process_t* tmp = proc;
        while(tmp->next_state != NULL){
            tmp = tmp->next_state;
        }
        process_t* first = proc;
        process_t* second = proc;
        first = tmp;
        second = second->next_state;
    }
}

int elec_create(void(*f),int n,realtime_t* start, realtime_t* deadline){
    int* process_stack_pointer = process_stack_init(f,n);
    if(process_stack_pointer == NULL){
        return -1;
    }
    process_t* new_state = malloc(sizeof(process_t));
    new_state->sp = process_stack_pointer;
	new_state->orig_sp = process_stack_pointer;
    if (deadline == NULL){ 
        new_state->next_state = NULL;
	    new_state->size = n;
	    new_state->start_time = NULL;
	    new_state->deadline_time = NULL;
        enqueue_nonRT(new_state);
        return -1;
    }
    else{
        new_state->start_time = start;
	    //convert deadline time to non-relative time
	    new_state->deadline_time = deadline;
	    new_state->deadline_time->sec += start->sec;
	    new_state->deadline_time->msec += start->msec;
	    if(new_state->deadline_time->msec > 999){
		    new_state->deadline_time->msec -= 1000;
		    new_state->deadline_time->sec += 1;
	    }
	    enqueue_RT_waiting(new_state);
	    return 1;
    }
}
