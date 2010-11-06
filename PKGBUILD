# Maintainer: schuay <jakob.gruber@gmail.com>

pkgname=pcurses
pkgver=0.0.0
pkgrel=1
pkgdesc="A curses package browser using libalpm"
arch=('x86_64' 'i686')
url="https://github.com/schuay/pcurses"
license=('GPL')
depends=('ncurses' 'pacman')
makedepends=('boost' 'qt')
source=("https://github.com/downloads/schuay/$pkgname/$pkgname-$pkgver.tar.bz2")
md5sums=('5d1fa9913795f03e318b15f3df7f344f')

build() {
  cd "$srcdir"

  qmake
  make
}

package() {
  install -Dm755 "$srcdir"/$pkgname "$pkgdir"/usr/bin/$pkgname
}

# vim:set ts=2 sw=2 et:
