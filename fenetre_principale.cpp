#include "fenetre_principale.h"
#include "ui_fenetre_principale.h"

FenetrePrincipale::FenetrePrincipale(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);

    /* Initialisation du territoire d'étude */
    ui->radioButton_commune->setChecked(true);
    QStringList liste_geographies = Territoire::Liste_libelles_geographies("Commune");
    ui->comboBox_geographie->addItems(liste_geographies);

    /* Initialisation de la période d'observation */
    Periode *p = new Periode();
    p->Dernier_millesime_majic();
    p->Ajustement_automatique_annee_debut();
    ui->spinBox_an_debut->setValue(p->Annee_debut());
    ui->spinBox_an_fin->setValue(p->Annee_fin());
    ui->spinBox_an_pas->setValue(p->Pas_de_temps());

    /* Validation des saisies utilisateur */
    connect(ui->pushButton_validation, SIGNAL(clicked(bool)), this, SLOT(Validation_des_saisies(bool)));

}

FenetrePrincipale::~FenetrePrincipale()
{
    delete ui;
}

void FenetrePrincipale::Validation_des_saisies(bool)
{
    /* Territoire d'étude */
    QPair<QString, QString> geographie;
    geographie.second = ui->comboBox_geographie->currentText();
    territoire = new Territoire("Commune", geographie);

    /* Période d'observation */
    periode = new Periode();
    periode->Annee_debut(ui->spinBox_an_debut->value());
    periode->Annee_fin(ui->spinBox_an_fin->value());
    periode->Pas_de_temps(ui->spinBox_an_pas->value());

    /* Calcul des occupations du sol par année */
    Algorithme *algorithme = new Algorithme(territoire, periode);

}

