/**
*Entradas
*
*presencia
*T
*Salidas
*
*luz
*
*Estados
*ON/OFF
*
 */

 ltl apagaLuces{
     []((T && (!presencia W !luz))-> <>!luz)
 }

 ltl enciendeLuces{
     [](presencia -> <>luz)
 }

 /*Inputs*/
 int presencia;
 int T;

 /*Outputs*/
 int luz;

 /*States*/

int luzState;


active proctype fsm(){

    
    luzState=0;

    do
        ::if
            ::(luzState==0)->atomic {
                if
                
                ::(presencia)->luzState=1; luz=1;presencia=0;
                printf("(presencia) Transition from state 0 to state 1\n");
                /*::(!presencia)->luzState=0;luz=0;presencia=0;
                printf("(presencia) Transition from state 0 to state 0\n");*/
                fi
            }
            ::(luzState==1)-> atomic{
                if
                ::(!presencia && T)->luzState=0;T=0;luz=0;presencia=0;
                printf("(!presencia && T) Transition from state 1 to state 0\n");
                ::(presencia)->luzState=1;presencia=0;T=0;luz=1;
                printf("(presencia) Transition from state 1 to state 1\n");
                fi
            }
        fi;
    printf ("estado = %d, presencia = %d, T = %d, luz = %d\n", 
	    luzState, presencia, T, luz) 
    od

}



active proctype entorno(){
    do
        
    :: skip->skip
    ::T=1
    ::presencia=1
    od
}