# Copyright 2013 Javier S. Pedro
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=4
EGIT_REPO_URI="git://gitorious.org/gato/libgato.git"

inherit git-2 qt4-r2

DESCRIPTION="Generic Attribute Profile library for Bluetooth devices"
HOMEPAGE="https://gitorious.org/firefox-gnome-globalmenu/qt4-gnome-globalmenu"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="x86 amd64"
IUSE="debug"

RDEPEND=">=dev-qt/qtcore-4.8:4 net-wireless/bluez"

src_unpack() {
	git-2_src_unpack
}
