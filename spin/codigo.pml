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

    []((!pulsador W esperaLarga) && codigo -> <>codigo_ok)
}

ltl espraDiezCodigoNo_ok{
    []((!pulsador W esperaLarga) && !codigo -> <>!codigo_ok)
}

/*Inputs*/
int pulsador;
int esperaCorta;
int esperaLarga;

/*Outputs*/
int codigo_ok;

/*Estados*/
int digitoState;
int posicionState;
int codigo;


active proctype fsm(){

}

active proctype entorno(){
    do
    ::skip->skip
    ::pulsador=1
    ::esperaLarga=1
    ::esperaCorta=1
    od
}