#include "SPIRIT/Concepts/Concepts.hpp"
#include "catch2/catch_test_macros.hpp"

struct A
{
};
struct B : public A
{
};
struct C : public A
{
};
struct D : public B
{
};


TEST_CASE("Concepts")
{
    SECTION("Bases")
    {
        using namespace sp::traits::details;

        // Access and manip
        REQUIRE(std::is_same<Bases<int, float>::first, int>::value);
        REQUIRE(
            std::is_same<Bases<int, float, double>::others, Bases<float, double>>::value
        );

        REQUIRE(std::is_same<Bases<int>::extend<float>, Bases<int, float>>::value
        );
        REQUIRE(std::is_same<Bases<>::extend<float>, Bases<float>>::value);

        ////////////////////////////////////////////////////////////
        // Bases of
        ////////////////////////////////////////////////////////////

        REQUIRE(std::is_same<Bases<A>::BasesOf<B, C, D>::Bases_t, Bases<A>>::value
        );
        REQUIRE(
            std::is_same<Bases<A, B>::BasesOf<B, C, D>::Bases_t, Bases<A>>::value
        );

        REQUIRE(
            std::is_same<Bases<A, B>::BasesOf<B, D>::Bases_t, Bases<A, B>>::value
        );
        REQUIRE(
            std::is_same<Bases<A, B, C, D>::BasesOf<B, D>::Bases_t, Bases<A, B>>::value
        );
        REQUIRE(
            std::is_same<Bases<A, B, C, D>::BasesOf<D>::Bases_t, Bases<A, B, D>>::value
        );
        REQUIRE(
            std::is_same<Bases<A, B, C, D>::BasesOf<A, B, C, D>::Bases_t, Bases<A>>::value
        );

        ////////////////////////////////////////////////////////////
        // Deepest Base
        ////////////////////////////////////////////////////////////

        REQUIRE(std::is_same<Bases<A, B>::Deepest::Deepest_t, B>::value);
        REQUIRE(std::is_same<Bases<A, B, D>::Deepest::Deepest_t, D>::value);

        // Doesnt work right now, A is discarded... but in our use case,
        // B and C are expected to be mutually exclusive, thus we would never
        // get this.
        // => Deepest works only on linear inheritance chains
        // REQUIRE(std::is_same<Bases<A, B, C>::Deepest::Deepest_t, A>::value);
        // REQUIRE(std::is_same<Bases<A, B, C, D>::Deepest::Deepest_t, A>::value);

        ////////////////////////////////////////////////////////////
        // Deepest Base of Ts
        ////////////////////////////////////////////////////////////

        REQUIRE(
            std::is_same<Bases<A>::DeepestBaseOf<A, B, C, D>::DeepestBaseOf_t, A>::value
        );
        REQUIRE(std::is_same<
                Bases<A, B, C, D>::DeepestBaseOf<A, B, C, D>::DeepestBaseOf_t,
                A>::value);
        
        REQUIRE(
            std::is_same<Bases<A, B, D>::DeepestBaseOf<B, D>::DeepestBaseOf_t, B>::value
        );
 
        REQUIRE(
            std::is_same<Bases<A, B, C, D>::DeepestBaseOf<D>::DeepestBaseOf_t, D>::value
        );
    }
}