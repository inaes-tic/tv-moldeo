#include "supereg/tmotor.h"
 

//****************************************************************************************


TEngine_Utility::TEngine_Utility()
{
    //Aca tendria que haber una funcion del tipo cargar el supereg.ini
		Luzx = 8.0 / sqrt(129.0);Luzy = - 8.0 / sqrt(129.0);Luzz = -1.0 / sqrt(129.0);
		Inhibir = 0;
		SafeMode = 0;
    MULTALT = 1.0;
    Memoria_Utilizada = 0;
	Rapidez = 20000;
    RapidezMaxima = 100*100000;

    TG_RELIEVE = new TGrid_Utility(ESCALAS,ANCHO);
	TG_NUBES = new TGrid_Utility(7,(9+8+8));

    if(TG_RELIEVE==NULL) exit(0);
	if(TG_NUBES==NULL) exit(0);

}


//****************************************************************************************


TEngine_Utility::~TEngine_Utility()
{
 delete TG_RELIEVE;
}



//****************************************************************************************

//****************************************************************************************


void TEngine_Utility::Start_Engine(void)
{
  int FIN;
	FIN = 0;
  //TG_RELIEVE->Posicionar((TG_RELIEVE->Mapa->MapaAncho / 2),(TG_RELIEVE->Mapa->MapaAlto / 2),(float) 100000.0,&CAM3D);
  TG_RELIEVE->Posicionar(10,20,(float) 5000.0,&CAM3D);
  //TG_NUBES->Posicionar(10,20,(float) 5000.0,&CAM3D);
}

//******************

void TEngine_Utility::Loop_Engine(void)
{
     TG_RELIEVE->ActualizarGrillas(Rapidez,&CAM3D);
     TG_RELIEVE->TiempoCorriendo();
     //TG_RELIEVE->ProyectaryDibujar(pagina_virtual,&CAM3D);
	 TG_RELIEVE->glProyectaryDibujar(pagina_virtual,&CAM3D);

//nota:
	 //si la camara se encuentra entre las nubes  el relieve, entonces primero
	 //se dibuja primero las nubes y luego el relieve
	 //si se esta por encima de las nubes es alreves
	 //aunque lo mejor es tirar todo junto

     
	 //TG_NUBES->ActualizarGrillas(Rapidez,&CAM3D);
     //TG_NUBES->TiempoCorriendo();
     //TG_RELIEVE->ProyectaryDibujar(pagina_virtual,&CAM3D);
	 //TG_NUBES->glProyectaryDibujar(pagina_virtual,&CAM3D);
	 

}

//******************


//****************************************************************************************

void TEngine_Utility::Stop_Engine(void)
{
//nada
	Rapidez = 0;
}



//****************************************************************************************

void
TEngine_Utility::ImprimirTodaLaInfo(void)
{
//nada
}

//****************************************************************************************

void
TEngine_Utility::Trim(int val)
{
	float vf;
	vf = 15 *(float) val /(float) 256.0;
	turnview(-vf * PI/150,0.0,0.0,&CAM3D);	
	
}

//****************************************************************************************

void
TEngine_Utility::Pitch(int val)
{
	float vf;
	vf = 15 *(float) val /(float) 256.0;
	turnview(0.0,0.0, vf * PI/150,&CAM3D);
	
}

//****************************************************************************************


int
TEngine_Utility::Check_Keyb(int code)
{
//  TGrid *Temp;
//reescribir
//aca hay que usar SDL
       if(code==0)
           turnview(PI/150,0.0,0.0,&CAM3D);

       if(code==1)
           turnview(-PI/150,0.0,0.0,&CAM3D);

       if(code==2)
           turnview(0.0,0.0,PI/150,&CAM3D);

       if(code==3)
           turnview(0.0,0.0,-PI/150,&CAM3D);

       if(code==4)
           turnview(0.0,PI/50,0.0,&CAM3D);

       if(code==5)
           turnview(0.0,-PI/50,0.0,&CAM3D);

       if(code==6)
           {
            if(Rapidez <= 0) Rapidez = 1;
            else Rapidez = Rapidez * 2;
            if(Rapidez>RapidezMaxima) Rapidez = RapidezMaxima;
           }

       if(code==7)
           {
           if(Rapidez<1) Rapidez = 0;
           else Rapidez = Rapidez / 2;
           }
/*
       if(key[KEY_ESC])
           return(-1);

       if(key[KEY_I]) CAM3D.foco-=7;
       if(key[KEY_O]) CAM3D.foco+=7;

       if(key[KEY_Q]) CAM3D.foco=-CAM3D.foco;

       if(key[KEY_Y]) {
                        Luzx = CAM3D.u.x; Luzy = CAM3D.u.y; Luzz = CAM3D.u.z;
                        //aca deberia actualizar el vector deluz en todas las grillas
                        Temp = TG_RELIEVE->Inicial;
                        while(Temp!=NULL) {
                           Temp->Luzx = Luzx;
                           Temp->Luzy = Luzy;
                           Temp->Luzz = Luzz;
                           Temp = Temp->Hijo;
                           }
                        }
       if(key[KEY_1]) {if(Inhibir<ESCALAS) Inhibir++;}
       if(key[KEY_2]) {if(Inhibir>0) Inhibir--;}
*/

return(0);

}



