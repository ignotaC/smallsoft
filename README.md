# Small Soft pack
Made for God Glory.
Hail Supreame God one in great trinity.
Prise Saint Maria, mother of God.

A pack of my little unix programs.

1. coin - Program lets you find an answer
          to a 0/1 problem.
2. getproxies - Program filters few webpages
                to suck up proxies, also checks
                if they work properly. Data is
                saved to file "proxylist" in work dir.
                Program uses wget, so it is needed
                to work.
3. novena - It will save file in home directory
            and track with it day of novena.
            All you need to do is enter the name
            of current novena, also you can
            set time, if you started two days
            ago the prayer - still will work fine.
            Creates .novena file in env HOME
4. workout - Program generates workout entries
             for each day. Keep training!
             Creates .workout file in env HOME
5. logtime - Logs time from stdin per line and
             attaches it on stdout.
6. recev - Record events from mouse and keyboard.
           It needs to work xinput, and xmodmap.
           Also xev might be useful. Record before
           processing (all happen in one run) 
           might make a big file. You tell the
           program key you choose to start/stop
           recording and file where he saves
           the record. To play recorded events
           use playev program from this list.
7. playev - Program playes events saved in file
            created by recev program. In general
            you just use it like this:
            playev recev_file x y z
            Where ofc first arg is file name of
            event file, *x* is how many times
            you want to repeat playing event
            record, 0 means repeat forever.
            The *y* is time before starting to
            play event loop and *z* is time
            between every event record replaying.
8. wh_frktcp - Program sets up chosen ammoun of
               processess that will run an internet
               command using tcp. It will use torsocks
               all times. So we keep privacy.
               This mean you need to install torsocks.
               Program will guide you how to use it.
9. wh_inchttpext - Program will go through specified
                   link incrementing http link each time,
                   in position you point like http://xxx.xx/%lld.
                   In the %lld will be put number. just run program
                   without arguments and it will guid you how to
                   use it. The program was made for extracting data
                   from websites. I find it sometimes useful.
                   It will try to find phrase in downloaded web
                   page source. 
10. wh_httpext - In general it's a string extractor form a file.
                 It has three basic commands. Pass to pass file
                 untill a string is met. Cut that find strings
                 in line and outputs on stdout string between them.
                 last command is loop. It will loop for ever last
                 command. Only has sens to use after cut command.
                 Ofc loop will end with eof at stdin. If you 
                 don't pass any command *help prompt* will appear. 
11. linsaftrdiff - You pass two file names. Program prints on stdout
                   all lines of first passed file after first different
                   line is met in other file.
12. fstrswp - Swap one string in stream to other, only files with lines
              shorter than 8192 for now. Perfect for some source fixing.
              For example when we need to swap malloc.h to stdlib.h.
              this would look like this:
              find src -type f -iname '*.c' -exec sh -c \
                   'cat {} | fstrswp '\''malloc.h'\'' '\''stdlib.h'\'' ' \
                   '| tmpfile; mv tmpfile {}' \;
              Could be without '\'' but what if we have spaces or worse?
13. parsekdump - This is wrote for openbsd ktrace output. Not sure how it
                 would worko on other oses. Anyway you ktrace your program
                 eventualy to keep track of forks ktrace -i program.
                 Than run parsekdump and it will try to turn kdump output
                 into lists of system calls and their percent time.
                 Basic PID/TID output makes one line per system call.
                 line containes:
                 1. System call
                 2. How many times system call was called and returned
                 3. Precent of all system calls times it took
                 4. Precent it took compared to recording time.
                 last one takes recording time as first and last appearance
                 of CALL or RET.
14. ualist - Program will output little list of user agents to stdout.
             You use it like this: ualist > my_list.
             It uses torsocks wget to download specific webpage.
             Program sleeps a bit and is slow. If you stop getting output
             restart tor to change the end point IP. The owner of
             page we get UA is a bit dramatic. So you need to be stealthy.

How to install?
Run make in makefile directory.
Compiled programs will go to the bin directory
inside the directory of your makefile.
In the end you just copy what ever you like
to your env PATH
