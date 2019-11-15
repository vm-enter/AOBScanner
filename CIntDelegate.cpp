#include "stdafx.h"

#include "CIntDelegate.hpp"

CIntDelegate::CIntDelegate(int min, int max, QObject *parent) : QItemDelegate(parent)
{
	this->m_nMin = min;
	this->m_nMax = max;
}

QWidget *CIntDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QLineEdit *lineEdit = new QLineEdit(parent);
	QIntValidator *validator = new QIntValidator(this->m_nMin, this->m_nMax, lineEdit);
	lineEdit->setValidator(validator);
	return lineEdit;
}