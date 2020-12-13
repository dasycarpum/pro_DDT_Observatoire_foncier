/**
 * \file      periode.h
 * \author    Roland VANDE MAELE
 * \date      8 décembre 2020
 * \brief     Caractérisation d'une période d'analyse
 *
 * \details    Cette classe va caractériser la période : année de début de l'analyse foncière, année de fin
 *             et pas de temps.
 *             Elle permet d'évaluer la date de fin sur base du dernier millésime MAJIC
 *             Et elle permet d'ajuster automatiquement la date de début pour maintenir un multiple du pas de temps .
 */

#ifndef PERIODE_H
#define PERIODE_H

#include <QDebug>

#include "fichier.h"

class Periode
{
    int annee_debut;
    int annee_fin;
    int pas_de_temps;
    int millesime;

public:
    Periode(){annee_debut = 1948; millesime = annee_fin = 0; pas_de_temps = 3;} /*!< Constructeur de la classe */
    ~Periode(){}

    void Annee_debut(int ad){annee_debut = ad;} /*!< Modifie l'année de début \param ad Année de début de l'analyse foncière <em> 1948, 2000, ... </em> */
    void Annee_fin(int af){annee_fin = af;} /*!< Modifie l'année de fin \param af Année de fin de l'analyse foncière <em> 2018, 2019, ...</em>*/
    void Pas_de_temps(int pdt){pas_de_temps = pdt;} /*!< Modifie le pas de temps \param pdt Pas de temps <em> 3, 1, ...</em> */
    int Annee_debut(void) const {return annee_debut;} /*!< Retourne l'année de début de l'analyse foncière <em> 1948, 2000, ...</em> */
    int Annee_fin(void) const {return annee_fin;} /*!< Retourne l'année de fin de l'analyse foncière <em> 2018, 2019, ...</em> */
    int Pas_de_temps(void) const {return pas_de_temps;} /*!< Retourne le pas de temps utilisé <em> 3, 1, ...</em> */
    int Millesime(void) const {return millesime;} /*!< Retourne le millésime des données <em> 2018, 2019, ...</em> */

    void Dernier_millesime_majic(void);
    void Ajustement_automatique_annee_debut(void);
};

#endif // PERIODE_H
