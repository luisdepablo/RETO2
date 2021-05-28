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

ltl check_codigo_ok{
    []( ( ( (codigo[0] == codigo_usado[0]) &&
            (codigo[1] == codigo_usado[1]) &&
            (codigo[2] == codigo_usado[2]) ) && (index == 2) && esperaCorta ) ->
        <> codigo_ok)
}

ltl check_codigo_no_ok{
    []( ( ( (codigo[0] != codigo_usado[0]) ||
            (codigo[1] != codigo_usado[1]) ||
            (codigo[2] != codigo_usado[2]) ) && (index == 2) && esperaCorta ) ->
        <> !codigo_ok)

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
byte codigo[3];
byte codigo_usado[3]={3,3,3}
byte contador;
int index;




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

    index=0;
    contador=0;
    do
            ::(codeState==1) -> atomic{
                if
                ::(pulsador && !esperaCorta && !esperaLarga)->
                    pulsador=0; contador=(contador+1)%10; codigo[index]=contador; 
                    printf("(pulsador) index=%d codigo=%d%d%d\n", index, codigo[0], codigo[1], codigo[2]);
                ::(esperaCorta && (index<2))->
                    esperaCorta=0; contador=0; index++;
                    printf("(esperaCorta) index=%d codigo=%d%d%d\n", index, codigo[0], codigo[1], codigo[2]);
                ::(esperaCorta && (index==2) &&
                   ((codigo_usado[0]==codigo[0]) && (codigo_usado[1]==codigo[1]) && (codigo_usado[2]==codigo[2])))->
                    esperaCorta=0; index=0; codigo[0]=0; codigo[1]=0; codigo[2]=0; codigo_ok=1;
                    printf("(esperaCorta codigo ok) index=%d codigo=%d%d%d\n", index, codigo[0], codigo[1], codigo[2]);
                ::(esperaCorta && (index==2) &&
                   !((codigo_usado[0]==codigo[0]) && (codigo_usado[1]==codigo[1]) && (codigo_usado[2]==codigo[2])))->
                    esperaCorta=0; index=0; codigo[0]=0; codigo[1]=0; codigo[2]=0; codigo_ok=0;
                    printf("(esperaCorta codigo not ok) index=%d codigo=%d%d%d\n", index, codigo[0], codigo[1], codigo[2]);
                ::(esperaLarga && !esperaCorta)->
                    esperaLarga=0; index=0; codigo[0]=0; codigo[1]=0; codigo[2]=0; codigo_ok=0;
                    printf("(esperaLarga) index=%d codigo=%d%d%d\n", index, codigo[0], codigo[1], codigo[2]);
                fi
            }
    od
}

active proctype entorno_simplificado(){
    do
    ::skip->skip
    ::pulsador=1;
    ::esperaLarga=1;
    ::esperaCorta=1;
    od
}

