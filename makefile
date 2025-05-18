# This file is part of Serendipity, an UCI chess engine written in Java.
#
# Copyright (C) 2024-2025  Shawn Xu <shawn@shawnxu.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

EXE = Serendipity-Dev
EMBEDDED_NET_LOCATION = ./Serendipity/src/main/resources/embedded.nnue
NET_NAME = net1.nnue
ifndef MAVEN_EXE
	MAVEN_EXE = mvn
endif

all:
ifdef EVALFILE
	cp $(EVALFILE) $(EMBEDDED_NET_LOCATION)
else
	$(MAKE) net
endif
ifdef JAVA_HOME
	JAVA_HOME=$(JAVA_HOME) $(MAVEN_EXE) -f ./Serendipity/pom.xml package
else
	$(MAVEN_EXE) -f ./Serendipity/pom.xml package
endif
	cat header.sh ./Serendipity/target/Serendipity-Test.jar > $(EXE)
	chmod +x $(EXE)
	cp ./Serendipity/target/Serendipity-Test.jar Serendipity.jar
net:
	wget -O $(EMBEDDED_NET_LOCATION) https://github.com/xu-shawn/Serendipity-Networks/raw/main/$(NET_NAME)
