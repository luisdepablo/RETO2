/**  
*Entradas
*botonLed
*botonAlarma
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
    []((!botonAlarma && presencia && alarmaState)->(<>alarma))
}

ltl alarmaDesactivada{
    []((!alarmaState)->(!alarma))

}

ltl activaAlarma{
    [](((!alarmaState) && botonAlarma)->(<>alarmaState))
}

ltl desactivaAlarma{
    []((alarmaState && botonAlarma)->(<>!alarmaState))
}

/* Inputs */
int presencia;
int botonAlarma; // pulsador activa o desactiva la alarma

/* Outputs */
int alarma;

/* State */
int alarmaState; // 0 -> alarmaApagado; 1 -> alarmaActiva

active proctype fsm(){
    /*Estado inicial */
    alarmaState=0;
    alarma=0;

    do
        ::if
            ::(alarmaState==0)->atomic {
                if
                
                ::(botonAlarma)->alarmaState=1; botonAlarma=0;alarma=0;
                ::(!botonAlarma)->alarmaState=0;
                fi
            }
            ::(alarmaState==1)-> atomic{
                if
                ::(!botonAlarma && presencia)->alarmaState=1;alarma=1;
                ::(botonAlarma)->alarmaState=0;botonAlarma=0;presencia=0;alarma=0;
                fi
            }
        fi;

    od

}

active proctype entorno(){
    do
    ::skip -> skip
    ::presencia=1
    ::botonAlarma=1
    od

}