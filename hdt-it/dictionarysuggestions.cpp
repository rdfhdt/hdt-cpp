#include "dictionarysuggestions.hpp"

#include "stringutils.hpp"

#include <HDTVocabulary.hpp>

#define NUM_SUGGESTIONS 10

DictionarySuggestions::DictionarySuggestions(QLineEdit *parent) :
    QObject(parent),
    editor(parent),
    manager(NULL)
{
    popup = new QTreeWidget;
    popup->setWindowFlags(Qt::Popup);
    popup->setFocusPolicy(Qt::NoFocus);
    popup->setFocusProxy(parent);
    popup->setMouseTracking(true);

    popup->setColumnCount(1);
    popup->setUniformRowHeights(true);
    popup->setRootIsDecorated(false);
    popup->setEditTriggers(QTreeWidget::NoEditTriggers);
    popup->setSelectionBehavior(QTreeWidget::SelectRows);
    popup->setFrameStyle(QFrame::Box | QFrame::Plain);
    popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->header()->hide();

    popup->installEventFilter(this);

    connect(popup, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            SLOT(doneCompletion()));

    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(500);
    connect(timer, SIGNAL(timeout()), SLOT(autoSuggest()));
    connect(editor, SIGNAL(textEdited(QString)), timer, SLOT(start()));
}

DictionarySuggestions::~DictionarySuggestions(){
    delete popup;
}

bool DictionarySuggestions::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj != popup)
        return false;

    if (ev->type() == QEvent::MouseButtonPress) {
        popup->hide();
        editor->setFocus();
        return true;
    }

    if (ev->type() == QEvent::KeyPress) {
        bool consumed = false;
        int key = static_cast<QKeyEvent*>(ev)->key();
        switch (key) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            doneCompletion();
            consumed = true;

        case Qt::Key_Escape:
            editor->setFocus();
            popup->hide();
            consumed = true;

        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Home:
        case Qt::Key_End:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            break;

        default:
            editor->setFocus();
            editor->event(ev);
            popup->hide();
            break;
        }

        return consumed;
    }

    return false;
}

void DictionarySuggestions::showCompletion(const QStringList &choices)
{
    if (choices.isEmpty())
        return;

    popup->setUpdatesEnabled(false);
    popup->clear();
    int maxWidth=0;
    QFontMetrics metrics(popup->font());

    for (int i = 0; i < choices.count(); ++i) {
        QTreeWidgetItem * item;
        item = new QTreeWidgetItem(popup);
        item->setText(0, choices[i]);
        maxWidth = qMax(maxWidth, metrics.boundingRect(choices[i]).width());
    }
    popup->setCurrentItem(popup->topLevelItem(0));
    popup->resizeColumnToContents(0);
    popup->adjustSize();
    popup->setUpdatesEnabled(true);

    int h = popup->sizeHintForRow(0) * qMin(NUM_SUGGESTIONS, choices.count()) + 3;
    popup->resize( qMax(popup->width(), qMin(maxWidth+20, 600)), h);

    popup->move(editor->mapToGlobal(QPoint(0, editor->height())));
    popup->setFocus();
    popup->show();
}

void DictionarySuggestions::doneCompletion()
{
    timer->stop();
    popup->hide();
    editor->setFocus();
    QTreeWidgetItem *item = popup->currentItem();
    if (item) {
        editor->setText(item->text(0));
        QMetaObject::invokeMethod(editor, "returnPressed");
    }
}

void DictionarySuggestions::autoSuggest()
{
    if(!manager ||!manager->hasHDT() ) {
        return;
    }

    try {
        QString str = editor->text();
        if(str.length()<1) {
            return;
        }
        QStringList choices;
        vector<string> options;

        // FETCH RESULTS FROM DICTIONARY
        manager->getHDT()->getDictionary().getSuggestions(str.toUtf8(), role, options, NUM_SUGGESTIONS);

        for(size_t i=0; i<options.size(); i++) {
            choices << stringutils::cleanN3String(options[i].c_str());
        }

        if(choices.count()==1 && choices[0]==str) {
            return;
        }

        // show
        showCompletion(choices);
    } catch (const char *ex) {
    } catch (char *ex) {
    }
}

void DictionarySuggestions::preventSuggest()
{
    timer->stop();
}

void DictionarySuggestions::setRole(hdt::TripleComponentRole role)
{
    this->role = role;
}

void DictionarySuggestions::setManager(HDTManager *manager)
{
    this->manager = manager;
}
