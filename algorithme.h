#ifndef ALGORITHME_H
#define ALGORITHME_H

#include "territoire.h"
#include "periode.h"

struct Usage
{
    double enaf;                //  espaces naturels, agricoles et forestiers
    double artificialise;       // bâti (habitat + non residentiel + dependance) et non bâti
    double habitat;             // indviduel et collectif
    double habitat_individuel;
    double habitat_collectif;
    double non_residentiel;
    double dependance;
    double non_bati;
    double a_batir;             // au sens de la DGFiP et non d'un document d'urbanisme

    Usage(){enaf = artificialise = habitat = habitat_individuel = habitat_collectif =
                    non_residentiel = dependance = non_bati = a_batir = 0.0;}
    ~Usage(){}
};

class Algorithme
{
    double surface_ign;
    Usage foncier;
    QMap<int, Usage> usages_par_annee;

public:
    Algorithme(const Territoire *, const Periode *);
    ~Algorithme(){}

    double Surface_ign(void) const {return surface_ign;}
    Usage Foncier(void) const {return foncier;}

    QMap<int, Usage> Usages_par_annee(void) const {return usages_par_annee;}
};

#endif // ALGORITHME_H