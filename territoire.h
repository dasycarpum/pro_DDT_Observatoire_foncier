#ifndef TERRITOIRE_H
#define TERRITOIRE_H

#include <QString>
#include <QPair>
#include <QStringList>

#include "fichier.h"

const QString DEPARTEMENT = "57";
const int HA = 10000;

/* Fichier "Communes" */
const int champ_departement = 3;
const int champ_code_commune = 1;
const int champ_libelle_commune = 8;


class Territoire
{
    QString granularite;                // plus petit grain = commune; sinon EPCI, SCoT, département
    QPair<QString, QString> geographie; // code, libellé
    QStringList liste_codes_INSEE_communes;

public:
    Territoire(QString const&, QPair<QString, QString> const&);
    ~Territoire(){}

    QStringList Liste_codes_INSEE_communes(void) const {return liste_codes_INSEE_communes;}

    static QStringList Liste_libelles_geographies(QString const&);

};

#endif // TERRITOIRE_H
