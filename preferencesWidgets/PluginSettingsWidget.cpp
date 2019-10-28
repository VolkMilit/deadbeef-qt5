#include "PluginSettingsWidget.h"

#include <QDebug>

#include <QSpinBox>
#include <QSlider>
#include <QLineEdit>
#include <QFormLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QRegExp>

#include "include/parser.h"
#include "QFileRequester.h"
#include "QDoubleSlider.h"
#include "include/callbacks.h"

extern ddb_dsp_context_t *current_dsp_context;
extern ddb_dsp_context_t *dsp_chain;

PluginSettingsWidget::PluginSettingsWidget(ddb_dialog_t *conf, QWidget *parent):
        QGroupBox(parent),
        layout(new QFormLayout(parent))
{
    setTitle(tr("Settings"));
    isDsp = false;
    configureWidgets(conf);
    setLayout(layout);
}

PluginSettingsWidget::PluginSettingsWidget(ddb_dsp_context_t *dsp, QWidget *parent):
        QGroupBox(parent)
{
    ddb_dialog_t conf;
    current_dsp_context = dsp;
    conf.title = dsp->plugin->plugin.name;
    conf.layout = dsp->plugin->configdialog;
    conf.get_param = dsp_conf_get_str;
    setTitle(tr("Settings"));
    isDsp = true;
    configureWidgets(&conf);
}

PluginSettingsWidget::~PluginSettingsWidget()
{
    current_dsp_context = NULL;

    if (dsp_chain)
        DBAPI->streamer_set_dsp_chain(dsp_chain);

    DBAPI->sendmessage(DB_EV_DSPCHAINCHANGED, 0, 0, 0);
}

void PluginSettingsWidget::configureWidgets(ddb_dialog_t *settingsDialog)
{
    //bool HLayout = false;
    
    //char token[MAX_TOKEN];
    const char *script = settingsDialog->layout;

    QStringList props = QString(script).split(";\n");

    // hack, check if we have two property without newline
    for (int i = 0; i < props.size(); i++)
    {
        if (props[i].contains(";property"))
        {
            QStringList tmp = props[i].split(";property");

            for (QString item : tmp)
            {
                if (item[0] == ';')
                    item.replace(0, 1, QChar());

                props.insert(i+1, item);
            }
        }
    }

    for (int i = 0; i < props.size(); i++)
    {
        QStringList prop = props[i].split(" ");
        QString desc = "";

        if (prop.size() < 5)
        {
            qDebug() << "Invalid property at line " << i+1;
            continue;
        }

        if (prop[1] == "box")
        {
            qDebug() << "[box] Stub at line " << i+1;
            continue;
        }

        for (int j = 0; j < prop.size(); j++)
        {
            if (!prop[0].contains("property"))
            {
                qDebug() << "Unknown property on line " << i+1;
                break;
            }

            if (prop[j][0] == '"')
            {
                for (int p = j; p < prop.size(); p++)
                {
                    if (prop[p].back() == '"')
                    {
                        desc += " " + prop[p];
                        break;
                    }
                    else
                    {
                        desc += " " + prop[p];
                    }

                    j++;
                }

                desc = desc.simplified().remove('"');
            }

            if (prop[j] == "checkbox")
            {
                QCheckBox *cb = new QCheckBox(this);
                cb->setText(desc);
                cb->setProperty("setting", prop[j + 1]);

                if (prop[j + 2] == "1")
                    cb->setCheckState(Qt::Checked);

                layout->addWidget(cb);
            }

            if (prop[j] == "entry")
            {
                QHBoxLayout *l = new QHBoxLayout;
                QLabel *descLabel = new QLabel;
                QLineEdit *le = new QLineEdit;

                descLabel->setText(desc);
                le->setProperty("setting", prop[j + 1]);
                le->setText(prop[j + 2]);

                l->addWidget(descLabel);
                l->addWidget(le);

                layout->addItem(l);
            }

            if (prop[j] == "file")
            {
                QHBoxLayout *l = new QHBoxLayout;
                QLabel *descLabel = new QLabel;
                QFileRequester *rqs = new QFileRequester(prop[j + 2], this);

                descLabel->setText(desc);

                l->addWidget(descLabel);
                l->addWidget(rqs);

                layout->addItem(l);
            }

            if (prop[j].contains("select["))
            {
                int size = prop[j][7].digitValue();

                QString setting = prop[j+1];
                int defaultIndex = prop[j+2].toInt();

                QComboBox *cb = new QComboBox(this);

                for (int e = 0; e < size; e++)
                {
                    cb->addItem(prop[(j+3)+e]);
                }

                j += size;

                cb->setCurrentIndex(defaultIndex-1);
                cb->setProperty("setting", setting);

                layout->addWidget(cb);
            }
        }
    }

    /*while ((script = gettoken(script, token)))
    {
        if (strcmp(token, ""))
            continue;

        if (strcmp(token, "property"))
        {
            qDebug() << "invalid token while loading plugin " << settingsDialog->title << " config dialog: " << token << " at line " << parser_line;
            break;
        }

        char labeltext[MAX_TOKEN];
        script = gettoken_warn_eof(script, labeltext);
        if (!script)
            break;
        
        char type[MAX_TOKEN];
        script = gettoken_warn_eof(script, type);
        if (!script)
            break;
        
        int spacing = 0;
        if (!strncmp (type, "hbox[", 5) || !strncmp (type, "vbox[", 5))
        {
            if (type[0] == 'h')
            {
                if (!HLayout && layout->isEmpty())
                {
                    HLayout = true;
                    delete layout;
                    layout = new QHBoxLayout(this);
                }
            }
            
            char param[MAX_TOKEN];
            int height;

            for (;;)
            {
                script = gettoken_warn_eof (script, param);

                if (!script)
                    break;

                if (!strcmp (param, ";"))
                {
                    break;
                }
                else if (!strncmp (param, "spacing=", 8))
                {
                    if (HLayout)
                    {
                        spacing = atoi (param+8);
                        layout->setContentsMargins(5, 0, 0, 0);
                        //layout->setSpacing(spacing+15);
                    }
                }
                else if (!strncmp (param, "height=", 7))
                {
                    height = atoi (param+7);
                    QWidget *parnetDlg = this->parentWidget();
                    parnetDlg->resize(parnetDlg->width(), height+120);
                }
            }


            //char semicolon[MAX_TOKEN];
            //while ((script = gettoken_warn_eof(script, semicolon)))
            //    if (!strcmp (semicolon, ";"))
            //        break;
            continue;
        }
        
        // ignore layout options
        //int vertical = 0;
        char key[MAX_TOKEN];
        const char *skiptokens[] = { "vert", NULL };

        for (;;)
        {
            script = gettoken_warn_eof(script, key);
            if (!strcmp (key, "vert")) {
                //vertical = 1;
            }

            int i = 0;

            for (int j = 0; skiptokens[i]; j++)
            {
                if (!strcmp (key, skiptokens[j]))
                    break;
            }

            if (!skiptokens[i])
                break;
        }
        
        if (!script)
            break;

        char def[MAX_TOKEN];
        script = gettoken_warn_eof(script, def);
        if (!script)
            break;

        char value[1000];
        settingsDialog->get_param(key, value, sizeof (value), def);

        if (!strcmp(type, "entry") || !strcmp(type, "password"))
        {
            label = new QLabel(tr(labeltext), this);
            prop = new QLineEdit(value, this);
            QLineEdit *lineEdit = qobject_cast<QLineEdit *>(prop);

            if (!strcmp(type, "password"))
                lineEdit->setEchoMode(QLineEdit::Password);

            addEntryWithLabel(layout, label, prop, HLayout);
            connect(lineEdit, &QLineEdit::editingFinished, this, &PluginSettingsWidget::saveProperty);
        }
        else if (!strcmp(type, "checkbox"))
        {
            prop = new QCheckBox(tr(labeltext), this);
            QCheckBox *checkBox = qobject_cast<QCheckBox *>(prop);
            int val = atoi(value);
            checkBox->setChecked(val);
            addEntry(layout, prop, HLayout);
            connect(checkBox, &QCheckBox::toggled, this, &PluginSettingsWidget::saveProperty);
        }
        else if (!strcmp(type, "file"))
        {
            label = new QLabel(tr(labeltext), this);
            //label->setWordWrap(true);
            //label->setMaximumWidth(400);
            prop = new QFileRequester(QString(value), this);
            addEntryWithLabel(layout, label, prop, HLayout);
            // TODO
            //connect(prop, QOverload<void>(&QFileRequester::changed), this, &PluginSettingsWidget::saveProperty);
        }
        else if (!strncmp(type, "select[", 7))
        {
            int n;

            if (1 != sscanf(type+6, "[%d]", &n))
                break;

            label = new QLabel(tr(labeltext), this);
            prop = new QComboBox(this);
            QComboBox *comboBox = qobject_cast<QComboBox *>(prop);

            for (int i = 0; i < n; i++)
            {
                char entry[MAX_TOKEN];
                script = gettoken_warn_eof(script, entry);

                if (!script)
                    break;

                comboBox->addItem(entry);
            }

            if (!script)
                break;

            comboBox->setCurrentIndex(atoi(value));
            addEntryWithLabel(layout, label, prop, HLayout);
            connect(comboBox, SIGNAL(currentIndexChanged(int)), SLOT(saveProperty()));
        }
        else if (!strncmp(type, "hscale[", 7) || !strncmp(type, "vscale[", 7) || !strncmp(type, "spinbtn[", 8))
        {
            float min, max, step;
            const char *args;

            if (type[0] == 's')
                args = type + 7;
            else
                args = type + 6;

            if (3 != sscanf(args, "[%f,%f,%f]", &min, &max, &step))
                break;

            //int invert = 0;
            if (min >= max)
            {
                float tmp = min;
                min = max;
                max = tmp;
                //invert = 1;
            }

            if (step <= 0)
                step = 1;

            if (type[0] == 's')
            {
                prop = new QDoubleSpinBox(this);
                QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>(prop);
                spinBox->setMaximum(max);
                spinBox->setMinimum(min);
                spinBox->setSingleStep(step);
                spinBox->setValue(atof(value));
                connect(spinBox, SIGNAL(editingFinished()), SLOT(saveProperty()));
            }
            else
            {
                prop = type[0] == 'h' ? new QDoubleSlider(Qt::Horizontal, 1/step, this) : new QDoubleSlider(Qt::Vertical, 1/step, this);
                QDoubleSlider *slider = qobject_cast<QDoubleSlider *>(prop);
                //slider->setInvertedAppearance(invert);
                slider->setTickPosition(QSlider::TicksBelow);
                slider->setTickInterval(std::max(max/10, step)/step);
                slider->setMaximum(max);
                slider->setMinimum(min);
                slider->setSingleStep(step);
                slider->setValue(atof(value));
                connect(slider, SIGNAL(sliderReleased()), SLOT(saveProperty()));
            }

            label = new QLabel(tr(labeltext), this);
            if (HLayout)
                label->setMinimumWidth(50+spacing);
            addEntryWithLabel(layout, label, prop, HLayout);
            //if (vertical)
        }

        if (prop)
            keys.insert(prop, key);

        script = gettoken_warn_eof(script, token);
        if (!script)
            break;

        if (strcmp (token, ";"))
        {
            qDebug() << "expected `;' while loading plugin " << settingsDialog->title << " config dialog: "<< token << " at line " << parser_line;
            break;
        }
    }*/
}

void PluginSettingsWidget::addEntry(QLayout *layout, QWidget *prop, bool HLayout)
{
    if (HLayout)
    {
        QHBoxLayout *hBoxLayout = qobject_cast<QHBoxLayout *>(layout);
        hBoxLayout->addWidget(prop);
    }
    else
    {
        QFormLayout *formLayout = qobject_cast<QFormLayout *>(layout);
        formLayout->addRow(prop);
    }
}

void PluginSettingsWidget::addEntryWithLabel(QLayout *layout, QLabel *label, QWidget *prop, bool HLayout)
{
    if (HLayout)
    {
        QHBoxLayout *hBoxLayout = qobject_cast<QHBoxLayout *>(layout);
        QVBoxLayout *vbox = new QVBoxLayout(nullptr);
        vbox->addWidget(prop);
        vbox->addWidget(label);
        hBoxLayout->addLayout(vbox);
    }
    else
    {
        QFormLayout *formLayout = qobject_cast<QFormLayout *>(layout);
        formLayout->addRow(label, prop);
    }
}
void PluginSettingsWidget::killDialog()
{
    saveProperty();
    qobject_cast<QDialog *>(this->parentWidget())->accept();
}

void PluginSettingsWidget::saveProperty()
{
    if (QWidget *widget = qobject_cast<QWidget *>(QObject::sender()))
    {
        QString val = "";

        if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget))
            val = lineEdit->text();
        else if (QCheckBox *checkBox = qobject_cast<QCheckBox *>(widget))
            val = checkBox->isChecked() ? "1" : "0";
        else if (QComboBox *comboBox = qobject_cast<QComboBox *>(widget))
            val = QString("%1").arg(comboBox->currentIndex());
        else if (QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>(widget))
            val = QString("%1").arg(spinBox->value());
        else if (QDoubleSlider *slider = qobject_cast<QDoubleSlider *>(widget))
            val = QString("%1").arg(slider->value());
        else if (QFileRequester *fileRequester = qobject_cast<QFileRequester *>(widget))
            val = QString("%1").arg(fileRequester->text());
        
        if (isDsp && current_dsp_context)
        {
            if (!val.isEmpty())
                current_dsp_context->plugin->set_param(current_dsp_context, keys.value(widget).toInt(), val.toUtf8().constData());
        }
        else
        {
            DBAPI->conf_set_str(keys.value(widget).toUtf8().constData(), val.toUtf8().constData());
            DBAPI->sendmessage(DB_EV_CONFIGCHANGED, 0, 0, 0);
        }
    }
}
