#include "fichier.h"

/**
 * \brief      Constructeur de la classe
 * \param      nom  Accès au fichier csv, incluant le chemin relatif depuis la position de l'exécutable, et le libellé du fichier p.ex. \a "/databank/territoire/code_libelle"
 * \param      sep  Séparateur de champs (par défaut le ';' )
 */

FichierCsv::FichierCsv(QString const& nom, QString const& sep) : extension(".csv"), separateur(sep)
{
    setFileName(QCoreApplication::applicationDirPath() + nom + extension);
}

/**
 * \brief      Lit le fichier csv et stocke les données dans une matrice 2D
 * \details    Lecture ligne par ligne, chacune scindée ensuite en colonnes par le biais du séparateur,
 *             pour fournir les attributs qui vont construire une matrice 2D, indexée [ligne][colonne]
 * \param      void
 * \return     void
 */
void FichierCsv::Lire(void)
{
    if (!open(QIODevice::ReadOnly | QIODevice::Text))
        QMessageBox::critical(nullptr, "Oups !", "Impossible d'ouvrir le fichier " + fileName());

    int l(0); // numéro de ligne dans le fichier
    while (!atEnd()) {
        QString enregistrement = readLine();    // lecture du fichier ligne par ligne
        enregistrement.remove(QRegExp("\\n"));  // suppression du caractère de fin de ligne
        matrix.resize(l+1);                     // redimensionnement du tableau d'une ligne supplémentaire

        QStringList champs = enregistrement.split(separateur);  // scission de la ligne en une table de champs
        matrix[l].resize(champs.size());        // redimensionnement du tableau avec le nombre de colonnes

        int c(0); // numéro de colonne dans l'enregistrement
        foreach (QString const& attribut, champs) { // complétude du tableau
            matrix[l][c] = attribut;
            ++c;
        }
        ++l;
    }
}
/**
 * \brief      Liste les sous-répertoires du dossier spécifié en paramètre
 * \param      rep  Chemin relatif du répertoire depuis la position de l'exécutable p.ex. \a "/databank/"
 * \return     Liste des libellés des sous-répertoires du dossier
 */
QStringList FichierCsv::Liste_sous_repertoires(const QString & rep)
{
    QStringList liste;

    QDir dir;
    if (rep.mid(1,1) != ":")
        dir.setPath(QCoreApplication::applicationDirPath() + rep);
    else
        dir.setPath(rep);

    QFileInfoList listRepertoire = dir.entryInfoList(QDir::Dirs);

    for (int i(0); i < listRepertoire.size(); ++i)
    {
        QFileInfo fileInfos = listRepertoire.at(i);

        if (fileInfos.baseName().size() > 0)
            liste.append(fileInfos.baseName());
    }

    return liste;
}
