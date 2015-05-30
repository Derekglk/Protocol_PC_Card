#ifdef NS_WIN_FLAG
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#else
#include <jni.h>
#include <new>
#endif

#include "dsp_basic.h"
#include "dsp_func.h"

#include "dsplibrary.h"
#include "common_macro.hpp"

#include "tec_edf.h"
#include "main_var.h"
#include "main_func.h"

//Bibliothèques pour les sockets UDP et le multitache
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include <unistd.h>

#define WRITE(FILE, FORMAT, VARIABLE) fprintf(FILE, FORMAT, VARIABLE)
#define READ(FILE, FORMAT, VARIABLE) fscanf(FILE, FORMAT, &VARIABLE)
#define OPEN_INPUT(FILENAME) fopen(FILENAME, "rt")
#define OPEN_OUTPUT(FILENAME) fopen(FILENAME, "wt")

#define nb 750 // Nombre de paquets à lire du fichier


#define PCM_BITS 24
#define FIR_SIZE 96

intp working_sample[TEC_FRAME_LG_WB * 3]; //48k

intp working_sample2[TEC_FRAME_LG_WB * 3]; //48k
intp fir_samples[FIR_SIZE] = { -13996, -23862, -25653, -7769, 29632, 71366, 93759, 80432, 36518, -11277, -31816, -12967, 27259, 52971, 38834, -7473, -48917, -48710, -2984, 51959, 67346, 24176, -46009, -83177, -49338, 34183, 98096, 81388, -12206, -108378, -119461, -22107, 111563, 164381, 73362, -103565, -218435, -150831, 76854, 288655, 278308, -12746, -401297, -545164, -169150, 715803, 1763771, 2470165, 2470165, 1763771, 715803, -169150, -545164, -401297, -12746, 278308, 288655, 76854, -150831, -218435, -103565, 73362, 164381, 111563, -22107, -119461, -108378, -12206, 81388, 98096, 34183, -49338, -83177, -46009, 24176, 67346, 51959, -2984, -48710, -48917, -7473, 38834, 52971, 27259, -12967, -31816, -11277, 36518, 80432, 93759, 71366, 29632, -7769, -25653, -23862, -13996};
intp delay_bb_ai[FIR_SIZE];
intp delay_bb_ai2[FIR_SIZE];

//Déclaration de variables pour mesurer le temps d'exécution
clock_t t1, t2;

//Déclaration des verrous et conditions pour la synchronisation des threads 
pthread_cond_t cond[nb] = PTHREAD_COND_INITIALIZER; // Création de la condition 
pthread_mutex_t mutex[nb] = PTHREAD_MUTEX_INITIALIZER; // Création du mutex 


//Structure contenant le buffer
typedef struct rec {
  int buffer[nb][960];
  int start[nb];
  int k;
};


//|=======================================================================================
/// Method name: dspReset()
/// Description: Pre-processing
//|=======================================================================================
void dspReset(void)
{

	io_size = 0;
	outputData = NULL;
	get_param = true;


}

//|=======================================================================================
/// Method name: dspGetParam()
/// Description: Get parameters
//|=======================================================================================
void dspGetParam(void)
{

}

//|=======================================================================================
/// Method name: dspInit()
/// Description: Pre-processing
//|=======================================================================================
void dspInit(void)
{


	//AUDIO_LOGD("Begin %s()", __FUNCTION__);

	core_config_type_g = WB_CFG;

	switch ((int)core_config_type_g)
	{
	case 1:
		Blk_size_i = 160;
		break;
	case 2:
		Blk_size_i = 320;
		break;
	default:
		Blk_size_i = 960;
		break;
	}


	// NS initialisation
	//configuration();

	m_change_temp_min_tracking_b = false;

	/* Retrieve the API parameters ----------------------------------------- */
	ns_flag_g = ON;

	ns_estim_gamma1_g = 6144;
	ns_estim_gamma2_g = 512;
	ns_estim_alpha_max_g = 983;
	ns_estim_beta_min_g = 717;
	ns_estim_rho_g = 717;
	ns_past_snr_weighting_factor_g = 1278;
	ns_present_snr_weighting_factor_g = 20;

	/* Non-stationary noise estimation parameters */
	ns_noise_est_win_len_g = 160;
	ns_noise_est_att_factor_g = 717;
	ns_noise_est_th_ctl_g = 400;

	/* Post-filter parameters */
	ns_post_filter_flag_g = ON;
	ns_post_filter_threshold_g = 640;
	ns_post_filter_length_g = 10;

	/* AGC parameters */
	ns_period_detection_flag_g = ON;
	ns_window_length_seek_max_energy_g = Blk_size_i >> 1;
	ns_step_move_window_g = 2;
	ns_nb_max_window_g = 8;

	/* Default values */
	ns_temporal_min_track_period_g = 8;
	ns_temporal_min_track_depth_g = 5;
	ns_smallest_apriori_snr_g = 20;

	ctrl_s.ns_param_s.ns_flag_es = ns_flag_g;

	ctrl_s.ns_param_s.noise_estim_s.temporal_min_track_period_i = ns_temporal_min_track_period_g;
	ctrl_s.ns_param_s.noise_estim_s.temporal_min_track_depth_i = ns_temporal_min_track_depth_g;
	ctrl_s.ns_param_s.noise_estim_s.gamma1_i = ns_estim_gamma1_g;
	ctrl_s.ns_param_s.noise_estim_s.gamma2_i = ns_estim_gamma2_g;
	ctrl_s.ns_param_s.noise_estim_s.alpha_max_i = ns_estim_alpha_max_g;
	ctrl_s.ns_param_s.noise_estim_s.beta_min_i = ns_estim_beta_min_g;
	ctrl_s.ns_param_s.noise_estim_s.rho_i = ns_estim_rho_g;
	ctrl_s.ns_param_s.noise_estim_s.noise_est_win_len_i = ns_noise_est_win_len_g;
	ctrl_s.ns_param_s.noise_estim_s.noise_est_att_factor_i = ns_noise_est_att_factor_g;
	ctrl_s.ns_param_s.noise_estim_s.noise_est_th_ctl_i = ns_noise_est_th_ctl_g;

	ctrl_s.ns_param_s.noise_filter_s.smallest_apriori_snr_i = ns_smallest_apriori_snr_g;
	ctrl_s.ns_param_s.noise_filter_s.past_snr_weighting_factor_i = ns_past_snr_weighting_factor_g;
	ctrl_s.ns_param_s.noise_filter_s.present_snr_weighting_factor_i = ns_present_snr_weighting_factor_g;


	/* Post filtering */
	ctrl_s.ns_param_s.post_filter_s.ns_post_filter_flag = ns_post_filter_flag_g;
	ctrl_s.ns_param_s.post_filter_s.ns_post_filter_threshold_i = ns_post_filter_threshold_g;
	ctrl_s.ns_param_s.post_filter_s.ns_post_filter_length_i = ns_post_filter_length_g;

	/* Period Detection */
	ctrl_s.ns_param_s.period_detect_s.ns_period_detection_flag = ns_period_detection_flag_g;
	ctrl_s.ns_param_s.period_detect_s.ns_window_len_seek_max_nrj_i = ns_window_length_seek_max_energy_g;
	ctrl_s.ns_param_s.period_detect_s.ns_step_move_window_i = ns_step_move_window_g;
	ctrl_s.ns_param_s.period_detect_s.ns_nb_max_window_i = ns_nb_max_window_g;



	/* Initialization --- */
	ns_init(&ctrl_s.ns_param_s.noise_estim_s, &ctrl_s.ns_param_s.noise_filter_s, &ctrl_s.ns_param_s.post_filter_s, &ctrl_s.ns_param_s.period_detect_s);

	ctrl_s.ns_param_s.first_call_u1 = true;
	for (int j = 0; j < Blk_size_i; j++)
	{
		ctrl_s.ns_in_s.input_signal_pi[j] = 0;
		ctrl_s.ns_out_s.output_signal_pi[j] = 0;
		ctrl_s.ns_out_s.output_noise_gain_pi[j] = 0;
	}

}

//|=======================================================================================
/// Method name: dspPost()
/// Description: Post-processing
//|=======================================================================================
void dspPost(void)
{
}



//|=======================================================================================
    
//Thread de traitement
void *t_trait (void * buff){

  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  int udpSocket;

  int sortie[nb][TEC_FRAME_LG_WB*3];
  int tab[TEC_FRAME_LG_WB*3]; //Tableau pour mettre en forme les échantillons avant de les envoyer

  rec *r;
  r=static_cast<rec *>(buff); // Cast du pointeur void* en rec*

  int n1=0,n2=0,i;

  long clk_tck = CLOCKS_PER_SEC;

    //Creation de la socket UDP
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

    //Initialisation de l'adresse du client
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr =33597632; // Ici l'adresse du client. En c++ on n'utilise pas la fonction inet_addr("IP") on écrit directement la valeur de l'adresse IP
    serverAddr.sin_port = htons(7892);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

    //Initialisation de la taille de l'adresse du client
    addr_size = sizeof serverAddr; 
   
	for (n1 = 0; n1 < nb; n1++)
	{
	  /*Verouillage du mutex et attente de la réception du paquet entier*/
             pthread_mutex_lock(&mutex[n1]);   
             while ((*r).start[n1] == 0) {
             pthread_cond_wait(&cond[n1], &mutex[n1]);
	     }
             pthread_mutex_unlock(&mutex[n1]);

	     //Débruitage trame par trame
               for (n2 = 0; n2 <TEC_FRAME_LG_WB*3; n2++) // Read 20 ms of signal @ 48k 
	       {
			long sample =(*r).buffer[n1][n2];
			
			//Convert sample from pcmformat to 24 bits format:
			sample = sample << (24 - PCM_BITS);

			//Set sample to processing interface: 
			//ctrl_s.ns_in_s.input_signal_pi[n2] = intp(sample);
			working_sample[n2] = (intp)sample;
		}

		// Downsample to 16k 
		dsp_fir_bb_processing(&delay_bb_ai[0], &working_sample[0], &fir_samples[0], &working_sample2[0], TEC_FRAME_LG_WB * 3, FIR_SIZE);

		for (n2 = 0; n2 < TEC_FRAME_LG_WB; n2++) // Output signal @96k
		{
			//Set sample to processing interface:
			ctrl_s.ns_in_s.input_signal_pi[n2] = working_sample2[n2 * 3];
		}		

		//NS processing per 20 ms of signal
		processing();

		//Upsample to 48k 
		for (n2 = 0; n2 < TEC_FRAME_LG_WB * 3; n2++) // Read 20 ms of signal @ 48k 
		{
			//Set sample to processing interface:
			working_sample2[n2] = 0;
		}
		for (n2 = 0; n2 < TEC_FRAME_LG_WB; n2++) // Read 20 ms of signal @ 48k 
		{
			//Set sample to processing interface:
			working_sample2[n2 * 3] = ctrl_s.ns_out_s.output_signal_pi[n2];
			
		}
		dsp_fir_bb_processing(&delay_bb_ai2[0], &working_sample2[0], &fir_samples[0], &working_sample[0], TEC_FRAME_LG_WB * 3, FIR_SIZE);
		//write_int_array(sortie2, (int *)&working_sample[0], TEC_FRAME_LG_WB * 3, 7); // right samples on 16bits 


        //Mise en forme des échantillons pour l'envoi
		for(i=0;i<960;i++){
		  tab[i]=working_sample[i] >> 7;
		}

		//Envoi du paquet débruité
		sendto(udpSocket,tab,960*sizeof(int),0,(struct sockaddr *)&serverAddr,addr_size);	     
	     

        //Mesure et affichage du temps d'exécution
		if(n1==(nb-1)){
		  t2 = clock();

	        }
        }

    printf("Nombre de paquets envoyés %d \n", n1);
    printf("Temps consomme (s) : %lf \n",(double)(t2-t1)/(double)clk_tck);
}   

//|=======================================================================================

//Thread de reception     
void *t_rec (void * buff){
  
    rec *r;
    r=static_cast<rec *>(buff);   // Cast du pointeur void* en rec*
    int p=sizeof(long);
    printf("%d \n",p);
    int udpSocket, nBytes;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    
    //Création de la socket UDP
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

    //Initialisation de l'adresse du serveur 
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr =1677764800; // Ici c'est l'adesse du serveur. En c++ on n'utilise pas la fonction inet_addr("IP") on écrit directement la valeur de l'adresse IP.
    serverAddr.sin_port = htons(7891);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	//Association de l'adresse du serveur à la socket créée
    bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
   
    //Réception des paquets
    while((*r).k<nb){

      //Verouillage du mutex en attente de la réception complète du paquet*/
        pthread_mutex_lock(&mutex[(*r).k]);
        nBytes = recvfrom(udpSocket,(*r).buffer[(*r).k],960*sizeof(int),0,NULL,NULL);
		printf("recu %d\n",(*r).k);

        //Instant de réception du premier paquet
		if((*r).k==0){
			t1 = clock();
		}

        (*r).start[(*r).k]=1;

		//Déverouillage du mutex
        pthread_cond_signal(&cond[(*r).k]); 
        pthread_mutex_unlock(&mutex[(*r).k]);
  
        (*r).k=(*r).k+1;
    } 
    
    printf("Nombre de paquets recus %d \n", ((*r).k));
   
}    




//|=======================================================================================
/// Method name: main()
/// Description: Main processing
//|=======================================================================================


int main(int argc, char *argv[])
{
    //Initialisation 
    dspReset();
    dspInit();
    dsp_fir_init(&delay_bb_ai[0], FIR_SIZE);
    dsp_fir_init(&delay_bb_ai2[0], FIR_SIZE);
 
    int i;	

    pthread_t mont_rec;
    pthread_t mont_trait;

    //Création et initialisation du buffer
    rec *args =static_cast<rec *> (malloc(sizeof *args));
    (*args).k=0;
    for(i=0;i<nb;i++){
      (*args).start[i]=0;
    }
    
    //Céation des threads
    pthread_create(&mont_trait,NULL,t_trait,args);
    pthread_create(&mont_rec,NULL,t_rec,args);

    pthread_join (mont_rec, NULL);
    pthread_join (mont_trait, NULL);

    return 0;
}






int open_text_input(FILE ** fptr, char * fileName)
{
	int r;
	*fptr = OPEN_INPUT(fileName);

	r = !(*fptr);

	if (r) printf("Error in opening file %s\n", fileName);

	return -r;
}

int open_text_output(FILE ** fptr, char * fileName)
{
	int r;

	*fptr = OPEN_OUTPUT(fileName);

	r = !(*fptr);

	if (r) printf("Error in opening file %s\n", fileName);

	return -r;
}
void close_io(FILE * fptr)
{
	fclose(fptr);
}

/*int types*/
void write_int_array(FILE * fptr, int * arr, int length, int shift)
{
	int i;

	for (i = 0; i<length; i++)
	{
		WRITE(fptr, "%d\n", arr[i] >> shift);
	}
}

void read_int_array(FILE * fptr, int * arr, int length)
{
	int i;

	for (i = 0; i<length; i++)
	{
		READ(fptr, "%d", arr[i]);
	}
}

void write_int_scalar(FILE * fptr, int scalar)
{
	WRITE(fptr, "%d\n", scalar);
}

int read_int_scalar(FILE * fptr)
{
	int value;

	READ(fptr, "%d", value);

	return value;

}
