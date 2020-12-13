/**
 * \file      algorithme.h
 * \author    Roland VANDE MAELE
 * \date      8 décembre 2020
 * \brief     Méthode d'analyse des données foncières
 *
 * \details    Ce fichier contient une structure et une classe qui vont fournir la méthode pour traiter les fichiers MAJIC, et ainsi
 *             calculer l'occupatio du sol au moment du millésime des données, et l'évolution de ses usages durant
 *             la période définie.\n
 *             Pour cela, l'algorithme reçoit en entrée le territoire et la période spécifiés par l'utilisateur.
 */


#ifndef ALGORITHME_H
#define ALGORITHME_H

#include "territoire.h"
#include "periode.h"

/*! \struct     Usage algorithme.h
 *  \brief      Cette structure répertorie les usages distinctifs du sol.
 *
 *  \details    Les usages décrits sont une synthèse des différentes occupations du sol répertoriées
 *              dans le fichier \c pnb10_parcelle de MAJIC et complété avec des données de la BD Topo (IGN)\n
 *              On se réfèrera à l'exposé de la méthode <em> Indicateur de la mobilisation des espaces
 *              naturels, agricoles et forestiers au profit de l’artificialisation des sols </em>
 *              pour de plus amples détails.
 */

struct Usage
{
    double enaf;                /*!< Surface des espaces naturels, agricoles et forestiers en m2 */
    double artificialise;       /*!< Surface du bâti (habitat + non résidentiel + dépendance) et non bâti en m2 */
    double habitat;             /*!< Surface de l'habitat indviduel et collectif en m2 */
    double habitat_individuel;  /*!< Surface de l'habitat indviduel en m2 */
    double habitat_collectif;   /*!< Surface de l'habitat collectif en m2 */
    double non_residentiel;     /*!< Surface du bâti non résidentiel (commerce, industrie, militaire, ...) en m2*/
    double dependance;          /*!< Surface des dépendances isolées en m2 */
    double non_bati;            /*!< Surface artificialisée mais non bâtie en m2 */
    double a_batir;             /*!< Surface des terrains à bâtir (au sens de la DGFiP et non d'un document d'urbanisme) en m2 */

    /**
     * \brief   Constructeur de la classe
     * */
    Usage(){enaf = artificialise = habitat = habitat_individuel = habitat_collectif =
                    non_residentiel = dependance = non_bati = a_batir = 0.0;}
};

class Algorithme
{
    double surface_ign;
    Usage foncier;
    QMap<int, Usage> usages_par_annee;
    const Periode *periode;

public:
    Algorithme(const Territoire *, const Periode *);
    ~Algorithme(){}

    double Surface_ign(void) const {return surface_ign;}    /*!< Retourne la surface totale du territoire en m2 (donnée issue de la BD Topo de l'IGN) */
    Usage Foncier(void) const {return foncier;} /*!< Pour le territoire, retourne les \link Usage \endlink du sol au dernier millésime  */
    QMap<int, Usage> Usages_par_annee(void) const {return usages_par_annee;} /*!< Pour le territoire, retourne certains \link Usage \endlink du sol cumulés par année (<em> habitat_individuel, habitat_collectif ou non_residentiel </em>), en fonction de la \link Periode \endlink définie */

    QVector<QPointF> Bati_cumule(double Usage::*arg);
    QVector<QPointF> Bati_courant(double Usage::*arg);
    QVector<QPointF> Conso_Foncier_Cumul(void);
    QVector<QPointF> Conso_Foncier_Courant(QVector<QPointF> xy_cumul);

};

#endif // ALGORITHME_H
