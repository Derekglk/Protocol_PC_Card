clear all;
close all;
clc;
Frequence1= 1000;                        % Fréquence 2 kHz
A1= 1;                                   % Amplitude du signal 1
FrequenceEchant= 48*Frequence1;          % Fréquence d'échantillonnage des signaux
Tmax=1 ;                                 % Durée des signaux en seconde
pas= 1/FrequenceEchant;           % Pas d'échantilonnage
Nb_ech=Tmax/pas
t= 0:pas:Tmax-pas;                       % Vecteur temps
Signal1 =  A1*sin(2*pi*Frequence1*t);    % Signal 1
%plot(t,Signal1,'r')
plot (Signal1(1,1:100),'r');

%% Ecriture dans un fichier pour développement en C
%--------------------------------------------------
FidVoie1 = fopen('signal1.txt','wb');        % Ouverture du fichier 1 en écriture 

fwrite(FidVoie1, Signal1(1,:),'float');            % Ecriture au format double du signal 1 dans le fichier 1

fclose(FidVoie1);                                    % Fermeture des fichiers 1 
 
            