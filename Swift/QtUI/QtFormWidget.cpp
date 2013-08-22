/*
 * Copyright (c) 2010-2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/QtFormWidget.h>

#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QScrollArea>
#include <qdebug.h>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swiften/Base/foreach.h>
#include <boost/algorithm/string/join.hpp>
#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {

QtFormWidget::QtFormWidget(Form::ref form, QWidget* parent) : QWidget(parent), form_(form) {
	QGridLayout* thisLayout = new QGridLayout(this);
	int row = 0;
	if (!form->getTitle().empty()) {
		QLabel* instructions = new QLabel(("<b>" + form->getTitle() + "</b>").c_str(), this);
		thisLayout->addWidget(instructions, row++, 0, 1, 2);
	}
	if (!form->getInstructions().empty()) {
		QLabel* instructions = new QLabel(form->getInstructions().c_str(), this);
		thisLayout->addWidget(instructions, row++, 0, 1, 2);
	}
	QScrollArea* scrollArea = new QScrollArea(this);
	thisLayout->addWidget(scrollArea);
	QWidget* scroll = new QWidget(this);
	QGridLayout* layout = new QGridLayout(scroll);
	foreach (boost::shared_ptr<FormField> field, form->getFields()) {
		QWidget* widget = createWidget(field);
		if (widget) {
			layout->addWidget(new QLabel(field->getLabel().c_str(), this), row, 0);
			layout->addWidget(widget, row++, 1);
		}
	}
	scrollArea->setWidget(scroll);
	scrollArea->setWidgetResizable(true);
	setEditable(form->getType() != Form::CancelType && form->getType() != Form::ResultType);
}

QtFormWidget::~QtFormWidget() {

}

QListWidget* QtFormWidget::createList(FormField::ref field) {
	QListWidget* listWidget = new QListWidget(this);
	listWidget->setSortingEnabled(false);
	listWidget->setSelectionMode(field->getType() == FormField::ListMultiType ? QAbstractItemView::MultiSelection : QAbstractItemView::SingleSelection);
	std::vector<bool> selected;
	foreach (FormField::Option option, field->getOptions()) {
		listWidget->addItem(option.label.c_str());
		if (field->getType() == FormField::ListMultiType) {
			selected.push_back(!field->getValues().empty() && option.value == field->getValues()[0]);
		}
		else if (field->getType() == FormField::ListMultiType) {
			std::string text = option.value;
			selected.push_back(std::find(field->getValues().begin(), field->getValues().end(), text) != field->getValues().end());
		}

	}
	for (int i = 0; i < listWidget->count(); i++) {
		QListWidgetItem* item = listWidget->item(i);
		item->setSelected(selected[i]);
	}
	return listWidget;
}

QWidget* QtFormWidget::createWidget(FormField::ref field) {
	QWidget* widget = NULL;
	if (field->getType() == FormField::BooleanType) {
		QCheckBox* checkWidget = new QCheckBox(this);
		checkWidget->setCheckState(field->getBoolValue() ? Qt::Checked : Qt::Unchecked);
		widget = checkWidget;
	}
	if (field->getType() == FormField::FixedType) {
		QString value = field->getFixedValue().c_str();
		widget = new QLabel(value, this);
	}
	if (field->getType() == FormField::ListSingleType) {
		widget = createList(field);
	}
	if (field->getType() == FormField::TextMultiType) {
		QString value = field->getTextMultiValue().c_str();
		QTextEdit* textWidget = new QTextEdit(this);
		textWidget->setPlainText(value);
		widget = textWidget;
	}
	if (field->getType() == FormField::TextPrivateType) {
		QString value = field->getTextPrivateValue().c_str();
		QLineEdit* lineWidget = new QLineEdit(value, this);
		lineWidget->setEchoMode(QLineEdit::Password);
		widget = lineWidget;
	}
	if (field->getType() == FormField::TextSingleType) {
		QString value = field->getTextSingleValue().c_str();
		widget = new QLineEdit(value, this);
	}
	if (field->getType() == FormField::JIDSingleType) {
		QString value = field->getJIDSingleValue().toString().c_str();
		widget = new QLineEdit(value, this);
	}
	if (field->getType() == FormField::JIDMultiType) {
		QString text = boost::join(field->getValues(), "\n").c_str();
		QTextEdit* textWidget = new QTextEdit(this);
		textWidget->setPlainText(text);
		widget = textWidget;
	}
	if (field->getType() == FormField::ListMultiType) {
		widget = createList(field);
	}
	fields_[field->getName()] = widget;
	return widget;
}

Form::ref QtFormWidget::getCompletedForm() {
	Form::ref result(new Form(Form::SubmitType));
	foreach (boost::shared_ptr<FormField> field, form_->getFields()) {
		boost::shared_ptr<FormField> resultField = boost::make_shared<FormField>(field->getType());
		if (field->getType() == FormField::BooleanType) {
			resultField->setBoolValue(qobject_cast<QCheckBox*>(fields_[field->getName()])->checkState() == Qt::Checked);
		}
		if (field->getType() == FormField::FixedType || field->getType() == FormField::HiddenType) {
			resultField->addValue(field->getValues().empty() ? "" : field->getValues()[0]);
		}
		if (field->getType() == FormField::ListSingleType) {
			QListWidget* listWidget = qobject_cast<QListWidget*>(fields_[field->getName()]);
			if (listWidget->selectedItems().size() > 0) {
				int i = listWidget->row(listWidget->selectedItems()[0]);
				resultField->addValue(field->getOptions()[i].value);
			}
		}
		if (field->getType() == FormField::TextMultiType) {
			QTextEdit* widget = qobject_cast<QTextEdit*>(fields_[field->getName()]);
			QString string = widget->toPlainText();
			if (!string.isEmpty()) {
				resultField->setTextMultiValue(Q2PSTRING(string));
			}
		}
		if (field->getType() == FormField::TextPrivateType || field->getType() == FormField::TextSingleType || field->getType() == FormField::JIDSingleType) {
			QLineEdit* widget = qobject_cast<QLineEdit*>(fields_[field->getName()]);
			QString string = widget->text();
			if (!string.isEmpty()) {
				resultField->addValue(Q2PSTRING(string));
			}
		}
		if (field->getType() == FormField::JIDMultiType) {
			QTextEdit* widget = qobject_cast<QTextEdit*>(fields_[field->getName()]);
			QString string = widget->toPlainText();
			if (!string.isEmpty()) {
				QStringList lines = string.split("\n");
				foreach (QString line, lines) {
					resultField->addValue(Q2PSTRING(line));
				}
			}
		}
		if (field->getType() == FormField::ListMultiType) {
			QListWidget* listWidget = qobject_cast<QListWidget*>(fields_[field->getName()]);
			foreach (QListWidgetItem* item, listWidget->selectedItems()) {
				resultField->addValue(field->getOptions()[listWidget->row(item)].value);
			}
		}
		field->setName(field->getName());
		result->addField(resultField);
	}
	return result;
}

template<class T> void QtFormWidget::setEnabled(QWidget* rawWidget, bool editable) {
	T* widget = qobject_cast<T*>(rawWidget);
	if (widget) {
		widget->setEnabled(editable);
	}
}

template<class T> void QtFormWidget::setEditable(QWidget* rawWidget, bool editable) {
	T* widget = qobject_cast<T*>(rawWidget);
	if (widget) {
		widget->setReadOnly(!editable);
	}
}

void QtFormWidget::setEditable(bool editable) {
	if (!form_) {
		return;
	}
	foreach (boost::shared_ptr<FormField> field, form_->getFields()) {
		QWidget* widget = NULL;
		if (field) {
			widget = fields_[field->getName()];
		}
		setEnabled<QCheckBox>(widget, editable);
		setEnabled<QListWidget>(widget, editable);
		setEditable<QTextEdit>(widget, editable);
		setEditable<QLineEdit>(widget, editable);
	}
}

}
