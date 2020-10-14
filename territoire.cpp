#include "territoire.h"

Territoire::Territoire(QString const& gra, QPair<QString, QString> const& geo) : granularite(gra), geographie(geo)
{
    if (geographie.first.isEmpty()){

        /* N° de champ du code géographique pour la granularité choisie par l'utilisateur */
        int champ_lib_geo(0), champ_cod_com(0), champ_cod_dep(0);

        FichierCsv *g = new FichierCsv("/databank/territoire/granularite");
        g->Lire();

        for (int i(1); i < g->matrix.size(); ++i){
            if (g->matrix[i][0] == granularite)
                champ_lib_geo = g->matrix[i][2].toInt();
            if (g->matrix[i][0] == "Département")
                champ_cod_dep =  g->matrix[i][1].toInt();
            if (g->matrix[i][0] == "Commune")
                champ_cod_com =  g->matrix[i][1].toInt();
        }
qDebug() << champ_lib_geo << champ_cod_com << champ_cod_dep;
        FichierCsv *cl = new FichierCsv("/databank/territoire/code_libelle");
        cl->Lire();
qDebug() << granularite<< geographie.second;
        for (int i(1); i < cl->matrix.size(); ++i)
            if (cl->matrix[i][champ_cod_dep] == DEPARTEMENT && cl->matrix[i][champ_lib_geo] == geographie.second)
                liste_codes_INSEE_communes.append(cl->matrix[i][champ_cod_com]);

        delete cl;
        delete g;
    }

    qDebug() << liste_codes_INSEE_communes;
}

QStringList Territoire::Liste_libelles_geographies(QString const& granularite)
{
    QMap<QString, bool> liste_libelle;

    /* N° de champ des libellés géographiques pour la granularité choisie par l'utilisateur */
    int champ_lib_geo(0), champ_cod_dep(0);

    FichierCsv *g = new FichierCsv("/databank/territoire/granularite");
    g->Lire();

    for (int i(1); i < g->matrix.size(); ++i){
        if (g->matrix[i][0] == granularite)
            champ_lib_geo = g->matrix[i][2].toInt();
        if (g->matrix[i][0] == "Département")
            champ_cod_dep =  g->matrix[i][1].toInt();
    }

    /* Liste des libellés géographiques pour la granularité retenue */
    FichierCsv *cl = new FichierCsv("/databank/territoire/code_libelle");
    cl->Lire();

    for (int i(1); i < cl->matrix.size(); ++i)
        if (cl->matrix[i][champ_cod_dep] == DEPARTEMENT)
            liste_libelle[cl->matrix[i][champ_lib_geo]] = true;

    delete cl;
    delete g;

    return liste_libelle.keys();
}


