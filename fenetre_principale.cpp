#include "fenetre_principale.h"
#include "ui_fenetre_principale.h"

FenetrePrincipale::FenetrePrincipale(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);

    /* Initialisation du territoire */
    ui->radioButton_commune->setChecked(true);
    QStringList liste_geographies = Territoire::Liste_libelles_geographies("Commune");
    ui->comboBox_geographie->addItems(liste_geographies);


}

FenetrePrincipale::~FenetrePrincipale()
{
    delete ui;
}

