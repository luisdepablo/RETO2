/**  
*Entradas
*botonLed
*pulsador
*presencia
****************************
*Estados
*ledState
*alarmaState
****************************
*Salida
*alarma
*led
*/


ltl alarmaActivada{
    [](alarmaState->[]((presencia && !pulsador)-><>alarma))
  //  []((( presencia )->(<>alarma)))
  []((alarmaState&&presencia)-><>alarma)
}

ltl alarmaDesactivada{
    []((!alarmaState)->(!alarma))

}

ltl activaAlarma{
    []((!alarmaState && pulsador)-><>alarmaState)
}

ltl desactivaAlarma{
    []((alarmaState && pulsador)-><>!alarmaState)
}

/* Inputs */
int presencia;
int pulsador; // pulsador activa o desactiva la alarma

/* Outputs */
int alarma;

/* State */
int alarmaState; // 0 -> alarmaApagado; 1 -> alarmaActiva

active proctype fsm(){
    /*Estado inicial */
    alarmaState=0;
    

    do
        ::if
            ::(alarmaState==0)-> atomic{
                if
                
                ::(pulsador)->alarmaState=1; pulsador=0;    printf("(pulsador) Transition from state 0 to state 1\n");
                ::(!pulsador)->alarmaState=0;               printf("(!pulsador) Transition from state 0 to state 0\n");
                fi
            }
            ::(alarmaState==1)-> atomic{
                if
                ::(!pulsador && presencia)->alarmaState=1;alarma=1; printf("(!pulsador && presencia) Transition from state 1 to state 1\n");
                ::(pulsador)->alarmaState=0;pulsador=0;alarma=0;    printf("(pulsador) Transition from state 1 to state 0\n");
                fi
            }
        fi;

    od

}

active proctype entorno(){
    do
    ::skip -> skip
    ::presencia=1
    ::pulsador=1
    od

}