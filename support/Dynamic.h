//
// Created by Ben on 2019-02-06.
//

#ifndef MACROTEXT_DYNAMIC_H
#define MACROTEXT_DYNAMIC_H

//set up dynamic library filenaming.
#ifdef __MACH__
#define SL(p,x) (p+#x + std::string(".dylib"))
#define SO(x) (#x + std::string(".dylib"))
#else
#define SL(p,x) p+#x + std::string(".so")
#define SO(x) #x + std::string(".so")
#endif

#endif //MACROTEXT_DYNAMIC_H
