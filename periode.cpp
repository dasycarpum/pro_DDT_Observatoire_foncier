#include "periode.h"

/**
 * \brief      Evalue de la date de fin sur base du millésime MAJIC disponible
 * \details    A partir du libellé du sous-répertoire où sont stockés les fichiers MAJIC \a /databank/MAJIC_2019/
 *             la fonction en récupère la 2e partie, qui sera égale au millésime, et l'année de fin de l'analyse foncière,
 *             qui sera égale au millésime moins 1 (puisque le millésime est toujours clos au 1er janvier).
 *             Lors d'une actualisation, il sera nécessaire de modifier le nom du sous-répertoire en conséquence.
 * \param      void
 * \return     void
 */

void Periode::Dernier_millesime_majic(void)
{
    QStringList liste = FichierCsv::Liste_sous_repertoires("/databank/");

    foreach (QString const& ss_rep, liste)
        if (ss_rep.split('_').first() == "MAJIC"){
            millesime = ss_rep.split('_').last().toInt();
            annee_fin = millesime - 1;
        }
}

/**
 * \brief      Ajuste automatiquement la date de début
 * \details    A partir de la date de fin (voir \link Periode::Dernier_millesime_majic() \endlink), la fonction calcule selon le pas de temps, une date de début
 *             qui maintient un multiple entier [ (annee_fin - annee_debut) / pas_de_temps ].
 * \param      void
 * \return     void
 */

void Periode::Ajustement_automatique_annee_debut(void)
{
    int periode(annee_fin - annee_debut);

    while(periode % pas_de_temps != 0){
        annee_debut++;
        periode = annee_fin - annee_debut;
    }
}
