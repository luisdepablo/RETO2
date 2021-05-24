/*  Modelo para verificacion formal, incluye especificacion LTL.
    ISEL 2021 */

ltl enciende {
	[] (button -> <> light)
}

ltl apaga {
	[] ((deadline && (!button W !light)) -> <> !light)
}


/* FSM inputs (used for guards) */
int button;
int deadline;

/* FSM outputs */
int light;


/* Process that indicates FSM behavior */
active proctype fsm() {
  int state = 0;

  printf("Estado inicial: 0\n");
  printf ("estado = %d, button = %d, deadline = %d, light = %d\n", 
	    state, button, deadline, light)
  do
  ::if
    :: (state == 0)  -> atomic {
        if
        :: (button) ->  light = 1; state = 1; button = 0; 
			printf("(button) Transition from state 0 to state 1\n");
        fi
    }
    :: (state == 1) -> atomic {
        if
        :: (button) ->  button = 0; 
			printf("(button) Transition from state 1 to state 1\n");
        :: (deadline && !button) -> state = 0; light = 0; deadline = 0;
			printf("(deadline) Transition from state 1 to state 0\n");
        fi
    }
    fi;
    printf ("estado = %d, button = %d, deadline = %d, light = %d\n", 
	    state, button, deadline, light)
  od
}


active proctype entorno () {
	do
	:: skip -> skip
	:: button = 1
	:: deadline = 1
	od
}

