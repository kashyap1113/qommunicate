#include "ui_messagedialog.h"

class MessageDialog : public QDialog
{
    Q_OBJECT

public:
    MessageDialog(QString, QWidget *parent=0);

private:
    Ui::MessageDialog ui;
};
