MAKE = make
PLUGINS = todo datebook address calls-an todo-an event-db3 float-db3 \
          meeting-an supernames memo brainforest tealphone hi-note-picture \
          hi-note-text todoplus listmaker zipcodeplus fireviewer clipboard \
          tinyviewer spec diddle jshopper bigtodo event-visor float-visor \
          handbase cryptopad event-db4 float-db4 actioneer memoplus \
	  addressplus arranger pedit32 progect pedit peditPro doc todo-db4 \
	  phonebook wordsmith bmp event-db5 float-db5 todo-db5 png

LOCALIZATIONS = german french

install:
	for plug in $(PLUGINS) ; do \
	    cd $$plug; $(MAKE) install; \
	    for localz in $(LOCALIZATIONS) ; do \
	        $(MAKE) install-$$localz LANG=$$localz; \
	    done ; \
	    cd .. ; \
	done
	

	
clean:
	-rm -f *~
	for plug in $(PLUGINS) ; do \
	    cd $$plug; $(MAKE) veryclean; \
	    cd .. ; \
	done

