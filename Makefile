#
# File		Makefile
# Author	Heiko KLausing (h dot klausing at gmx dot de)
# Created	2020-09-29
# Note		Read installed extensions and restore them to VSCode
#

EXTENSIONFILE := VSCodeExtensions.txt

.DEFAULT_GOAL := help

## -- Handling of Visula Studio Code Extensions --

## Read installed Visual Studio Code extensions and store them
.PHONY: freeze-extensions
freeze-extensions:
	code --list-extensions > $(EXTENSIONFILE)

## Install stored Visual Studio Code extensions
.PHONY: install-extensions
install-extensions:
	cat $(EXTENSIONFILE) | xargs -L 1 code --install-extension

## -- User information --

## This help message
.PHONY: help
help:

	@printf "Usage:\n  make <\033[36mtargets\033[0m>\n";

	@awk '{ \
			if ($$0 ~ /^.PHONY: [a-zA-Z0-9_-]+$$/) { \
				helpCommand = substr($$0, index($$0, ":") + 2); \
				if (helpMessage) { \
					printf "\033[36m%-20s\033[0m %s\n", \
						helpCommand, helpMessage; \
					helpMessage = ""; \
				} \
			} else if ($$0 ~ /^[a-zA-Z0-9._-]+:/) { \
				helpCommand = substr($$0, 0, index($$0, ":")); \
				if (helpMessage) { \
					printf "\033[36m%-20s\033[0m %s\n", \
						helpCommand, helpMessage; \
					helpMessage = ""; \
				} \
			} else if ($$0 ~ /^##/) { \
				if (helpMessage) { \
					helpMessage = helpMessage"\n                     "substr($$0, 3); \
				} else { \
					helpMessage = substr($$0, 3); \
				} \
			} else { \
				if (helpMessage) { \
					print "\n                     "helpMessage"\n" \
				} \
				helpMessage = ""; \
			} \
		}' \
		$(MAKEFILE_LIST)
