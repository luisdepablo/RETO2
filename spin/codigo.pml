/**Entradas
*pulsador
*esperaCorta
*esperaLarga
*
*Salidas
*
*codigo_ok
*
*codigo
*digitoState
*posicionState
*
 */

ltl esperaDiezCodigo_ok{

    [](((  esperaLarga && code ) W !pulsador )-> <>codigo_ok)
}

ltl esperaDiezCodigoNo_ok{
    [](((esperaLarga && !code) W !pulsador) -> <>!codigo_ok)
}

/*Inputs*/
int pulsador;
int esperaCorta;
int esperaLarga;

/*Outputs*/
int codigo_ok;

/*Estados*/
int codeState;
//mtype{IDLE,digito1,digito2,digito3,comprabacion}


/*Variables*/
/* int d1;
int d2;
int d3;

int c1=3;
int c2=3;
int c3=3; */

int code;





active proctype fsm_simplificado(){
    codeState=1;

    do
        ::if
            ::(codeState==1) -> atomic{
                if
                ::(pulsador)->pulsador=0; printf("(pulsador)");
                ::(esperaLarga && code)->codeState=2;codigo_ok=1;printf("(esperaLarga && code)");
                ::(esperaLarga && !code)->codeState=2;codigo_ok=0;printf("(esperaLarga && !code)");
                fi
            }
            ::(codeState==2)-> atomic{
                if
                ::(1)->codeState=1; code=0; esperaLarga=0; codigo_ok=0;
                fi
            }
        fi;

    od
}

active proctype entorno_simplificado(){
    do
    ::skip->skip
    //::pulsador=1
    ::esperaLarga=1
    ::code=1
    od
}

/* active proctype fsm(){
    codeState=IDLE;
    do
        ::if
            ::(codeState==IDLE) -> atomic{
                if
                ::(pulsador)->codeState=digito1;pulsador=0;d1++;
                fi
            }
            ::(codeState==digito1)-> atomic{
                if
                ::(pulsador)->codeState=digito1;pulsador=0;d1++;
                ::(pulsador && d1==9)->codeState=digito1;pulsador=0;d1=0;
                ::(esperaCorta)->codeState=digito2;esperaCorta=0;
                ::(esperaLarga)->codeState=comprabacion;esperaLarga=0;
                fi

            }
            ::(codeState==digito2) -> atomic{
                if
                ::(pulsador)->codeState=digito2;pulsador=0;d2++;
                ::(pulsador && d2==9)->codeState=digito2;pulsador=0;d2=0;
                ::(esperaCorta)->codeState=digito3;esperaCorta=0;
                ::(esperaLarga)->codeState=comprabacion;esperaLarga=0;
                fi

            }
            ::(codeState==digito3) -> atomic{
                if
                ::(pulsador)->codeState=digito3;pulsador=0;d3++;
                ::(pulsador && d3==9)->codeState=digito3;pulsador=0;d3=0;
                ::(esperaCorta)->codeState=digito1;esperaCorta=0;
                ::(esperaLarga)->codeState=comprabacion;esperaLarga=0;
                fi


            }
            ::(codeState==comprabacion)-> atomic{
                if
                ::(d1==c1 && d2==c2 && d3==c3)-> codeState=comprabacion;d1=10;d2=10;d3=10;code=1;
                ::(d1!=c1 && d2!=c2 && d3!=c3)->codeState=comprabacion;d1=10;d2=10;d3=10;code=0;
                ::(code)->codeState=IDLE;code=0;d1=0;d2=0;d3=0;codigo_ok=1;
                ::(!code)->codeState=IDLE;d1=0;d2=0;d3=0;codigo_ok=0;
                fi

            }
        fi;

    od
    
} */


/* active proctype entorno(){
    do
    ::skip->skip
    ::pulsador=1
    ::esperaLarga=1
    ::esperaCorta=1
    od
} */