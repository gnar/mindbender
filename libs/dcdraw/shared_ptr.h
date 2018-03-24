#ifndef DCDRAW_SHARED_PTR_H
#define DCDRAW_SHARED_PTR_H

namespace DCDraw {
    namespace internal {
        template<class T>
        class shared_ptr {
            void share(T *p, unsigned *refc) {
                unshare();

                ptr = p;
                if (ptr) {
                    ref_counter = refc;
                    ++*ref_counter;
                } else {
                    ref_counter = 0;
                }
            }

            void unshare() {
                if (ptr) {
                    --*ref_counter;
                    if (*ref_counter == 0) {
                        delete ptr;
                        delete ref_counter;
                    }

                    ptr = 0;
                    ref_counter = 0;
                }
            }

        public:
            void reset(T *init = 0) {
                unsigned *tmp = init ? new unsigned(0u) : nullptr;

                unshare();

                ref_counter = tmp;
                if (init) *ref_counter = 1;
                ptr = init;
            }

            shared_ptr(T *init = 0) : ptr(0), ref_counter(nullptr) {
                reset(init);
            }

            shared_ptr(const shared_ptr<T> &other) : ptr(0), ref_counter(nullptr) {
                share(other.ptr, other.ref_counter);
            }

            ~shared_ptr() {
                unshare();
            }

            shared_ptr &operator=(const shared_ptr<T> &other) {
                share(other.ptr, other.ref_counter);
                return *this;
            }

            T *operator->() const {
                return ptr;
            }

            T &operator*() const {
                return *ptr;
            }

            T *get() const {
                return ptr;
            }

            unsigned uses() const {
                return ptr ? *ref_counter : 0;
            }

        private:
            T *ptr;
            unsigned *ref_counter;
        };
    }
}

#endif

