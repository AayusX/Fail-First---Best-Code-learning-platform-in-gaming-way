// Presentation Navigation System
let currentSlide = 0;
const slides = document.querySelectorAll('.slide');
const totalSlides = slides.length;
const navDotsContainer = document.getElementById('navDots');
const progressBar = document.getElementById('progressBar');
const prevBtn = document.getElementById('prevBtn');
const nextBtn = document.getElementById('nextBtn');

// Create navigation dots
function createNavDots() {
    for (let i = 0; i < totalSlides; i++) {
        const dot = document.createElement('div');
        dot.classList.add('nav-dot');
        if (i === 0) dot.classList.add('active');
        dot.addEventListener('click', () => goToSlide(i));
        navDotsContainer.appendChild(dot);
    }
}

// Update active slide
function updateSlide() {
    // Remove active class from all slides and dots
    slides.forEach(slide => slide.classList.remove('active'));
    document.querySelectorAll('.nav-dot').forEach(dot => dot.classList.remove('active'));

    // Add active class to current slide and dot
    slides[currentSlide].classList.add('active');
    document.querySelectorAll('.nav-dot')[currentSlide].classList.add('active');

    // Update progress bar
    const progress = ((currentSlide + 1) / totalSlides) * 100;
    progressBar.style.width = progress + '%';

    // Update navigation buttons
    prevBtn.style.opacity = currentSlide === 0 ? '0.3' : '1';
    nextBtn.style.opacity = currentSlide === totalSlides - 1 ? '0.3' : '1';
    prevBtn.style.cursor = currentSlide === 0 ? 'not-allowed' : 'pointer';
    nextBtn.style.cursor = currentSlide === totalSlides - 1 ? 'not-allowed' : 'pointer';

    // Scroll to slide
    slides[currentSlide].scrollIntoView({ behavior: 'smooth', block: 'start' });
}

// Go to specific slide
function goToSlide(index) {
    if (index >= 0 && index < totalSlides) {
        currentSlide = index;
        updateSlide();
    }
}

// Next slide
function nextSlide() {
    if (currentSlide < totalSlides - 1) {
        currentSlide++;
        updateSlide();
    }
}

// Previous slide
function prevSlide() {
    if (currentSlide > 0) {
        currentSlide--;
        updateSlide();
    }
}

// Keyboard navigation
document.addEventListener('keydown', (e) => {
    if (e.key === 'ArrowRight' || e.key === 'ArrowDown' || e.key === ' ') {
        e.preventDefault();
        nextSlide();
    } else if (e.key === 'ArrowLeft' || e.key === 'ArrowUp') {
        e.preventDefault();
        prevSlide();
    } else if (e.key === 'Home') {
        e.preventDefault();
        goToSlide(0);
    } else if (e.key === 'End') {
        e.preventDefault();
        goToSlide(totalSlides - 1);
    }
});

// Mouse wheel navigation (with debounce)
let isScrolling = false;
document.addEventListener('wheel', (e) => {
    if (isScrolling) return;

    isScrolling = true;
    setTimeout(() => {
        isScrolling = false;
    }, 800);

    if (e.deltaY > 0) {
        nextSlide();
    } else if (e.deltaY < 0) {
        prevSlide();
    }
}, { passive: true });

// Touch support for mobile
let touchStartY = 0;
let touchEndY = 0;

document.addEventListener('touchstart', (e) => {
    touchStartY = e.changedTouches[0].screenY;
}, { passive: true });

document.addEventListener('touchend', (e) => {
    touchEndY = e.changedTouches[0].screenY;
    handleSwipe();
}, { passive: true });

function handleSwipe() {
    const swipeThreshold = 50;
    const diff = touchStartY - touchEndY;

    if (Math.abs(diff) > swipeThreshold) {
        if (diff > 0) {
            // Swipe up
            nextSlide();
        } else {
            // Swipe down
            prevSlide();
        }
    }
}

// Button click handlers
prevBtn.addEventListener('click', prevSlide);
nextBtn.addEventListener('click', nextSlide);

// Intersection Observer for smooth animations
const observerOptions = {
    threshold: 0.3,
    rootMargin: '0px'
};

const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.style.opacity = '1';
            entry.target.style.transform = 'translateY(0)';
        }
    });
}, observerOptions);

// Observe all slides
slides.forEach(slide => {
    observer.observe(slide);
});

// Initialize
window.addEventListener('DOMContentLoaded', () => {
    createNavDots();
    updateSlide();

    // Add fade-in animation to elements
    const animatedElements = document.querySelectorAll('.feature-item, .feature-large, .step');
    animatedElements.forEach((el, index) => {
        el.style.opacity = '0';
        el.style.transform = 'translateY(30px)';
        setTimeout(() => {
            el.style.transition = 'all 0.6s ease';
            el.style.opacity = '1';
            el.style.transform = 'translateY(0)';
        }, index * 100);
    });
});

// Prevent default scroll behavior
document.addEventListener('scroll', (e) => {
    e.preventDefault();
}, { passive: false });

// Add smooth hover effects for images
document.querySelectorAll('.showcase-image').forEach(img => {
    img.addEventListener('mouseenter', function () {
        this.style.transition = 'transform 0.5s ease';
        this.style.transform = 'scale(1.05) rotate(1deg)';
    });

    img.addEventListener('mouseleave', function () {
        this.style.transform = 'scale(1) rotate(0deg)';
    });
});

// Add parallax effect on mouse move
document.addEventListener('mousemove', (e) => {
    const mouseX = e.clientX / window.innerWidth;
    const mouseY = e.clientY / window.innerHeight;

    document.querySelectorAll('.feature-item, .feature-large').forEach(item => {
        const speed = 5;
        const x = (mouseX - 0.5) * speed;
        const y = (mouseY - 0.5) * speed;
        item.style.transform = `translate(${x}px, ${y}px)`;
    });
});

// Add typing effect to main title on first slide
window.addEventListener('load', () => {
    const mainTitle = document.querySelector('.main-title');
    const text = mainTitle.textContent;
    mainTitle.textContent = '';
    mainTitle.style.opacity = '1';

    let i = 0;
    const typeWriter = () => {
        if (i < text.length) {
            mainTitle.textContent += text.charAt(i);
            i++;
            setTimeout(typeWriter, 150);
        }
    };

    setTimeout(typeWriter, 500);
});

// Add counter animation for stats (if needed in future)
function animateValue(element, start, end, duration) {
    let startTimestamp = null;
    const step = (timestamp) => {
        if (!startTimestamp) startTimestamp = timestamp;
        const progress = Math.min((timestamp - startTimestamp) / duration, 1);
        element.textContent = Math.floor(progress * (end - start) + start);
        if (progress < 1) {
            window.requestAnimationFrame(step);
        }
    };
    window.requestAnimationFrame(step);
}

// Console message
console.log('%c✨ FailFirst Presentation ✨', 'font-size: 20px; font-weight: bold; color: #d4af37;');
console.log('%cNavigation: Arrow keys, Mouse wheel, Touch swipe, or Navigation buttons', 'font-size: 12px; color: #2c3e50;');
