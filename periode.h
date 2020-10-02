#ifndef PERIODE_H
#define PERIODE_H

#include <QDebug>

#include "fichier.h"

class Periode
{
    int annee_debut;
    int annee_fin;
    int pas_de_temps;

public:
    Periode(){annee_debut = 1948; annee_fin = 0; pas_de_temps = 3;}
    ~Periode(){}

    void Annee_debut(int ad){annee_debut = ad;}
    void Annee_fin(int af){annee_fin = af;}
    void Pas_de_temps(int pdt){pas_de_temps = pdt;}
    int Annee_debut(void) const {return annee_debut;}
    int Annee_fin(void) const {return annee_fin;}
    int Pas_de_temps(void) const {return pas_de_temps;}

    void Dernier_millesime_majic(void);
    void Ajustement_automatique_annee_debut(void);
};

#endif // PERIODE_H
