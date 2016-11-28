#include "preferencestabdialog.h"
#include "ui_preferencestabdialog.h"

PreferencesTabDialog::PreferencesTabDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesTabDialog)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Remove question mark from title bar
    ui->setupUi(this);
}

PreferencesTabDialog::~PreferencesTabDialog()
{
    delete ui;
}
