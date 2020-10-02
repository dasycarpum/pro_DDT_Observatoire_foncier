#include "territoire.h"

Territoire::Territoire(QString const& gra, QPair<QString, QString> const& geo) : granularite(gra), geographie(geo)
{
    if (geo.first.isEmpty())
    {
        FichierCsv *f = new FichierCsv("/databank/territoire/" + granularite, ",");
        f->Lire();

        for (int i(1); i < f->matrix.size(); ++i)
            if (f->matrix[i][champ_departement] == DEPARTEMENT && f->matrix[i][champ_libelle] == geographie.second)
                liste_codes_INSEE_communes.append(f->matrix[i][champ_code_commune]);

        delete f;
    }
}

QStringList Territoire::Liste_libelles_geographies(QString const& granularite)
{
    QStringList liste;

    FichierCsv *f = new FichierCsv("/databank/territoire/" + granularite, ",");
    f->Lire();

    for (int i(1); i < f->matrix.size(); ++i)
        if (f->matrix[i][champ_departement] == DEPARTEMENT)
            liste.append(f->matrix[i][champ_libelle]);

    delete f;

    return liste;
}


