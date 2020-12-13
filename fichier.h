/**
 * \file      fichier.h
 * \author    Roland VANDE MAELE
 * \date      8 décembre 2020
 * \brief     Interface pour lire un fichier texte de type "csv"
 *
 * \details    Cette classe va gérer la lecture d'un fichier csv en fournissant au constructeur
 *             son accès complet (chemin, libellé, extension), ainsi que le type de séparateur de champs, par défaut un ';'
 *             Les éléments du tableau seront stockés dans une  matrice à 2 dimensions, au format chaîne de caractères
 *             Elle permet également d'établir une liste des sous-répertoires d'un dossier.
 */

#ifndef FICHIERTEXTE_H
#define FICHIERTEXTE_H

#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QMessageBox>

class FichierCsv : public QFile
{
    const QString extension;
    const QString separateur;

public:
    QVector<QVector<QString>> matrix;   /*!< Matrice de stockage des attributs, indexée [ligne][colonne] */

    FichierCsv(QString const&, QString const& = ";");
    ~FichierCsv(){}

    void Lire(void);
    static QStringList Liste_sous_repertoires(QString const&);
};

#endif // FICHIERTEXTE_H
