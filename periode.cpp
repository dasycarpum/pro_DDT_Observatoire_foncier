#include "periode.h"

/** Evaluation de la date de fin sur base du dernier millésime MAJIC
    ================================================================ */
void Periode::Dernier_millesime_majic(void)
{
    QStringList liste = FichierCsv::Liste_sous_repertoires("/databank/");

    foreach (QString const& ss_rep, liste)
        if (ss_rep.split('_').first() == "MAJIC"){
            millesime = ss_rep.split('_').last().toInt();
            annee_fin = millesime - 1;
        }
}

/** Ajustement de la date de début, en fonction de celle de fin et du pas de temps
 *  ============================================================================== */
void Periode::Ajustement_automatique_annee_debut(void)
{
    int periode(annee_fin - annee_debut);

    while(periode % pas_de_temps != 0){
        annee_debut++;
        periode = annee_fin - annee_debut;
    }
}
