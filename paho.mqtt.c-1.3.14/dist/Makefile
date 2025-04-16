VERSION=1.3.0

check:
	rpmlint -i dist/paho-c.spec
 # Run rpmlint on the spec file to check for packaging issues

rpm-prep:
	mkdir -p ${HOME}/rpmbuild/SOURCES/
# Create the SOURCES directory if it doesn't exist	

	tar --transform="s/\./paho-c-${VERSION}/" -cf ${HOME}/rpmbuild/SOURCES/v${VERSION}.tar.gz --exclude=./build.paho --exclude=.git --exclude=*.bz ./ --gzip
# Create a gzipped tarball for the source code

rpm: rpm-prep
	rpmbuild -ba dist/paho-c.spec
# Build the RPM package using rpmbuild and the spec file