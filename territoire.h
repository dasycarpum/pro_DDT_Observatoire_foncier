/**
 * \file      territoire.h
 * \author    Roland VANDE MAELE
 * \date      7 décembre 2020
 * \brief     Caractérisation d'un territoire d'étude
 *
 * \details    Cette classe va caractériser un territoire : le numéro de département, la granularité
 *             (commune, EPCI, SCoT, département) et la (les) géographie(s) disponible(s).
 *             Elle permet de lister le(s) code(s) INSEE de la (des) communes du territoire retenu par l'utilisateur.
 *             Et elle permet d'évaluer les coordonnées XY du rectangle contenant l'emprise du territoire instancié.
 */

#ifndef TERRITOIRE_H
#define TERRITOIRE_H

#include <QString>
#include <QPair>
#include <QStringList>
#include <QMap>
#include <QSet>

#include "fichier.h"

const int HA = 10000; /*!< 1 hectare = 10 000 m2 - constante utilisée pour la conversion des m2 en ha */

class Territoire
{
    const QString departement;
    const QString granularite;
    QPair<QString, QString> geographie;
    QStringList liste_codes_INSEE_communes;
    QPair<QSet<QString>, QList<double>> emprise_communale;

public:
    /* Constructeur et destructeur */
    Territoire(QString const&, QString const&, QPair<QString, QString> const&);
    ~Territoire(){}

    /* Accesseurs */
    QString Departement(void) const {return departement;} /*!< Retourne le n° du département \a "54", "57", ... */
    QString Granularite(void) const {return granularite;}  /*!< Retourne la granularité \a "Commune", "EPCI", "SCoT" ou "Département" */
    QPair<QString, QString> Geographie(void) const {return geographie;} /*!< Retourne la géographie d'une granularité, définie par la paire code et libellé : \a p.ex. ("57463", "Metz") */
    QStringList Liste_codes_INSEE_communes(void) const {return liste_codes_INSEE_communes;}/*!< Retourne la liste de(s) code(s) INSEE de la (des) communes du territoire */
    QPair<QSet<QString>, QList<double>> Emprise_communale(void) const {return emprise_communale;} /*!< Retourne la liste unique de(s) code(s) INSEE de(s) commune(s) du territoire \b et de l'emprise géographique */

    /* Fonction */
    void Evaluation_emprise_communale(void);

    /* Fonction statique */
    static QStringList Liste_libelles_geographies(QString const&, QString const&);
};

#endif // TERRITOIRE_H
