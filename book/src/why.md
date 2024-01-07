# Why TinyKart

> Note: this page is mostly for those more experienced, and are curious why TinyKart exists. If you're a student
> just looking to work on a TinyKart, disregard this page.

TinyKart may appear to be a bit redundant at first, so I want to justify why ISC created this program, and it's larger
goals.

## The Background

Before TinyKart, being onboarded into ISC was a rough process. New members were essentially just thrown unto a project
and left to fend for themselves. While this did yield a few generational talents that ended up leading the team through
each of its generations through their sheer motivation, this model ultimately limited the clubs scope.

Because of the classes at UofM Dearborn, it is functionally impossible for students under senior year to participate in
robotics purely off knowledge from courses. In the case of CS majors, _it is literally impossible_, as all robotics
courses are in a different department. This is why, if the club has no program, talent in ISC largely originates from
pure tenacity and the loyal efforts of generally about 3 or so current members acting as mentors. This leads to teams
around 8-10 in size.

## The Problem

So why is this a problem? After all, ISC existed for years as essentially just a small group and did pretty well.
While there are a few perspectives on this, I'll give mine as the software lead.

### Bus factor, or program stability

> _Bus factor (noun): the number of people that need to get hit by a bus before your project is completely doomed._

I'm stealing this term from the wonderful [Google SWE book](https://abseil.io/resources/swe-book/html/toc.html). Bus
factor refers to, rather crudely, the number of people on
a project that can be hit by a bus before the project collapses. This is really just another way of calling someone a
single point of failure, weather that be in their knowledge of some problem, or their organisational knowledge and
rapport.

ISC's tendency to have a small, highly capable, ingroup leads it to be highly vulnerable to SPOF. This is rather
self-explanatory, as with a small team it simply isn't feasible to distribute work among members, while the project is
so challenging that its basically luck to find people capable of understanding everything.

While one can trivially expect this to cause issues when the bus hits, like missing deadlines, it causes a more
existential issue.

#### Time is a bus

Every four years or so, people graduate. This means that, however careful you are, your SPOF _will happen_ eventually.
When your SPOF leaves, it's up to luck that someone puts in the time to take their place, only to become a new SPOF.

The answer to this issue is simple, of course - just train members.

In reality though, it's far more difficult. This is because the SPOF _is the only one who can train members_. This SPOF
is also the one who is making the project possible however, so it's unreasonable to expect them to both mentor a ton
of people and fulfill their work. This leads to a hideous cycle, where every four years the SPOF graduates, everyone
who doesn't have the ability to take their spot leaves, a new SPOF is created, the club finally seems to get traction,
they graduate, and the cycle continues. Indeed, this is the cycle the team has been in for the last four years.

**TL;DR, a small team means you cannot train members, which means you cannot do cool things.**

## The Solution

TinyKart is our answer to the above problem.

In its simplest form, TinyKart is a course designed to bring a ton of people into the labspace, and get them to at least
a base level understanding of robotics. From there, those that remain, for there will always be attrition, will have the
chance to work in a team and experiment with software on a project far simpler and less daunting than large projects.

The goal of this is to break the cycle by iteratively growing the club body year by year. Critically, this doesn't mean
that we want TinyKart to teach all that the team needs. Rather, it's just an interesting problem to get people in the
labspace and up to a certain level of knowledge. Once they go to learn the big projects, they should be able to avoid the
SPOF by means of numbers. Will this avoid a SPOF entirely? Probably not. But by virtue of pure numbers, it should be
easier to, at least, train up the next year of TinyKart members, to avoid the team completely falling apart when the SPOF
leaves.

With this in mind, TinyKart has a few major choices:
- It should have minimal prerequisites
- It should have fallbacks, so if someone fails they don't get left behind
- It should be doable _without meetings at all if required_
  - Hence, this book
- It should focus on getting people to work together at ISC first, before critically challenging people
  - This is why we don't cover localisation
- It should allow for more exploratory work later, after people are more committed to the club socially, and thus more
  willing to stick through hard parts

### Why not F1/10?

I want to briefly address why we didn't just use F1/10, since this question comes up a lot. While F1/10 is great,
it doesn't really meet the program needs above. For two main reasons.

First, It's just very expensive. The f1/10 platform is like $1500 all included, which is just crazy. TinyKart is around
$350 all things included, which is far more scalable for us.

Second, because we don't want to get too deep into robotics for sake of accessibility, we want to avoid ROS. I love ROS,
but it has a crazy learning curve, especially because you need to learn quite a lot of C++ to use it. Most of our new
members barely even know C++, and often have only had one programming course. Conveniently, they have also almost all 
at least heard of Arduino, so it makes sense for us to exploit that. Because of this we also have to run with microcontrollers,
which conveniently also simplifies the hardware considerably.

