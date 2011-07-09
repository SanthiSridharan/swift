/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "DelegateCommons.h"

#include <QtScaledAvatarCache.h>
#include <QFileInfo>

namespace Swift {


void DelegateCommons::drawElidedText(QPainter* painter, const QRect& region, const QString& text) {
	QString adjustedText(painter->fontMetrics().elidedText(text, Qt::ElideRight, region.width(), Qt::TextShowMnemonic));
	painter->drawText(region, Qt::AlignTop, adjustedText);
}

void DelegateCommons::paintContact(QPainter* painter, const QStyleOptionViewItem& option, const QColor& nameColor, const QString& avatarPath, const QIcon& presenceIcon, const QString& name, const QString& statusText) const {
	painter->save();
	QRect fullRegion(option.rect);
	if ( option.state & QStyle::State_Selected ) {
		painter->fillRect(fullRegion, option.palette.highlight());
		painter->setPen(option.palette.highlightedText().color());
	} else {
		painter->setPen(QPen(nameColor));
	}

	QRect presenceIconRegion(QPoint(farLeftMargin, fullRegion.top()), QSize(presenceIconWidth, fullRegion.height() - verticalMargin));

	int calculatedAvatarSize = presenceIconRegion.height();
	//This overlaps the presenceIcon, so must be painted first
	QRect avatarRegion(QPoint(presenceIconRegion.right() - presenceIconWidth / 2, presenceIconRegion.top()), QSize(calculatedAvatarSize, calculatedAvatarSize));

	QPixmap avatarPixmap;
	if (!avatarPath.isEmpty()) {
		QString scaledAvatarPath = QtScaledAvatarCache(avatarRegion.height()).getScaledAvatarPath(avatarPath);
		if (QFileInfo(scaledAvatarPath).exists()) {
			avatarPixmap.load(scaledAvatarPath);
		}
	}
	if (avatarPixmap.isNull()) {
		avatarPixmap = QPixmap(":/icons/avatar.png").scaled(avatarRegion.height(), avatarRegion.width(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	painter->drawPixmap(avatarRegion.topLeft() + QPoint(((avatarRegion.width() - avatarPixmap.width()) / 2), (avatarRegion.height() - avatarPixmap.height()) / 2), avatarPixmap);

	//Paint the presence icon over the top of the avatar
	presenceIcon.paint(painter, presenceIconRegion, Qt::AlignBottom | Qt::AlignHCenter);

	QFontMetrics nameMetrics(nameFont);
	painter->setFont(nameFont);
	int extraFontWidth = nameMetrics.width("H");
	int leftOffset = avatarRegion.right() + horizontalMargin * 2 + extraFontWidth / 2;
	QRect textRegion(fullRegion.adjusted(leftOffset, 0, 0/*-leftOffset*/, 0));

	int nameHeight = nameMetrics.height() + verticalMargin;
	QRect nameRegion(textRegion.adjusted(0, verticalMargin, 0, 0));

	DelegateCommons::drawElidedText(painter, nameRegion, name);


	painter->setFont(detailFont);
	painter->setPen(QPen(QColor(160,160,160)));

	QRect statusTextRegion(textRegion.adjusted(0, nameHeight, 0, 0));
	DelegateCommons::drawElidedText(painter, statusTextRegion, statusText);

	painter->restore();
}

QSize DelegateCommons::contactSizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/ ) const {
	int heightByAvatar = avatarSize + verticalMargin * 2;
	QFontMetrics nameMetrics(nameFont);
	QFontMetrics statusMetrics(detailFont);
	int sizeByText = 2 * verticalMargin + nameMetrics.height() + statusMetrics.height();
	//Doesn't work, yay! FIXME: why?
	//QSize size = (option.state & QStyle::State_Selected) ? QSize(150, 80) : QSize(150, avatarSize_ + margin_ * 2);
	//qDebug() << "Returning size" << size;
	return QSize(150, sizeByText > heightByAvatar ? sizeByText : heightByAvatar);
}

const int DelegateCommons::horizontalMargin = 2;
const int DelegateCommons::verticalMargin = 2;
const int DelegateCommons::farLeftMargin = 2;
const int DelegateCommons::avatarSize = 20;
const int DelegateCommons::presenceIconHeight = 16;
const int DelegateCommons::presenceIconWidth = 16;



}
